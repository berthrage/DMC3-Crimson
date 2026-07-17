# New-Detour.ps1 - Creates a new ASM detour file and optionally sets up C++ boilerplate
# Usage: Run from the project root: .\New-Detour.ps1

$ErrorActionPreference = "Stop"

# Wrap entire script so errors pause the terminal instead of closing it
try {

# ============================================================================
# User input
# ============================================================================

$featureName = Read-Host "Enter the feature name (e.g. FixBossCam, GuardGravity)"
if ([string]::IsNullOrWhiteSpace($featureName)) {
    Write-Error "Feature name cannot be empty."
    exit 1
}
if ($featureName -match '[^a-zA-Z0-9_]') {
    Write-Error "Feature name should only contain alphanumeric characters and underscores."
    exit 1
}

Write-Host ""
$detourCountStr = Read-Host "How many detours in this file? [1] (default=1)"
if ([string]::IsNullOrWhiteSpace($detourCountStr)) { $detourCountStr = "1" }
if (-not [int]::TryParse($detourCountStr, [ref]$null) -or [int]$detourCountStr -lt 1) {
    Write-Error "Must be a positive integer."
    exit 1
}
$detourCount = [int]$detourCountStr

Write-Host ""
Write-Host "ASM template style?" -ForegroundColor Cyan
Write-Host "  [1] Minimal (just jmp return + .DATA/.CODE/CommonMacros)"
Write-Host "  [2] Full skeleton (commented patterns for C++ call, conditional jump, etc.)"
$asmStyle = Read-Host "Choice [1/2] (default=1)"
if ([string]::IsNullOrWhiteSpace($asmStyle)) { $asmStyle = "1" }
if ($asmStyle -notin @("1", "2")) { $asmStyle = "1" }
$fullAsm = ($asmStyle -eq "2")

# ============================================================================
# Collect per-detour data: CE line, source function, parse offsets/sizes
# ============================================================================

# Helper: parse a CE instruction line into offset, size, and formatted comment
function Parse-CE-Line {
    param([string]$line)
    $result = @{ Offset = "0x0"; Size = 0; Comment = "" }
    if ([string]::IsNullOrWhiteSpace($line)) { return $result }

    if ($line -match 'dmc3\.exe\s*\+\s*([0-9A-Fa-f]+)') {
        $offsetVal = $matches[1]
        $result.Offset = "0x$offsetVal"
    }
    if ($line -match 'dmc3\.exe\s*\+\s*[0-9A-Fa-f]+\s*-\s*([0-9A-Fa-f]+(?:\s+[0-9A-Fa-f]+)*)\s*-') {
        $bytesStr = $matches[1]
        $rawChunks = $bytesStr -split '\s+'
        $properBytes = @()
        foreach ($chunk in $rawChunks) {
            $upper = $chunk.ToUpper()
            for ($j = 0; $j -lt $upper.Length; $j += 2) {
                $properBytes += $upper.Substring($j, [Math]::Min(2, $upper.Length - $j))
            }
        }
        $result.Size = $properBytes.Count
        $formattedBytes = $properBytes -join ' '
        $afterBytes = $line -replace '.*?\s*-\s*[0-9A-Fa-f]+(?:\s+[0-9A-Fa-f]+)*\s*-\s*', ''
        $result.Comment = "dmc3.exe+$offsetVal - $formattedBytes - $afterBytes"
    }
    elseif ($line -match 'dmc3\.exe\s*\+\s*([0-9A-Fa-f]+)') {
        $offsetVal = $matches[1]
        $result.Comment = "dmc3.exe+${offsetVal}"
    }
    return $result
}

$detours = @()
$prevSourceFunc = ""

for ($d = 1; $d -le $detourCount; $d++) {
    # Determine suffix: no suffix for single detour, numbered for multi
    if ($detourCount -eq 1) {
        $suffix = ""
        $label = $featureName
    } else {
        $suffix = "$d"
        $label = "${featureName}${d}"
    }

    $detourName  = "${featureName}Detour${suffix}"
    $returnVar   = "g_${featureName}_ReturnAddr${suffix}"
    $checkCallVar = "g_${featureName}CheckCall${suffix}"  # only used in comments
    $hookVar     = "$([char]::ToLower($featureName[0]))$($featureName.Substring(1))Hook${suffix}"

    Write-Host ""
    Write-Host "=== Detour $d of $detourCount ($detourName) ===" -ForegroundColor Cyan

    $dCeLine = Read-Host "  Paste CE instruction line (or Enter to skip)"
    $ceParsed = Parse-CE-Line $dCeLine

    # Source function: always ask for first, cascade for subsequent
    if ($d -eq 1) {
        $dSourceFunc = Read-Host "  Source function name (e.g. CPlayerStaggeGravity_sub_1401FBE20, or Enter to skip)"
    } else {
        $sameFunc = Read-Host "  Same source function as detour $($d-1)? [Y/n] (default=Y)"
        if ($sameFunc -eq 'n' -or $sameFunc -eq 'N') {
            $dSourceFunc = Read-Host "  Source function name (or Enter to skip)"
        } else {
            $dSourceFunc = $prevSourceFunc
        }
    }
    $prevSourceFunc = $dSourceFunc

    $detours += @{
        Number      = $d
        Suffix      = $suffix
        Label       = $label
        DetourName  = $detourName
        ReturnVar   = $returnVar
        CheckCallVar = $checkCallVar
        HookVar     = $hookVar
        Offset      = $ceParsed.Offset
        Size        = $ceParsed.Size
        CEComment   = $ceParsed.Comment
        FromFunc    = $dSourceFunc
    }
}

# ============================================================================
# Derived paths
# ============================================================================

$scriptDir    = Split-Path -Parent $MyInvocation.MyCommand.Path
$srcDir       = Join-Path $scriptDir "src"
$masmDir      = Join-Path $srcDir "masm"
$cmakeFile    = Join-Path $srcDir "CMakeLists.txt"
$asmFile      = Join-Path $masmDir "$featureName.asm"
$funcName     = $featureName

# ============================================================================
# 1. Create .asm file
# ============================================================================

if (Test-Path $asmFile) {
    Write-Error "File $asmFile already exists! Aborting to avoid overwrite."
    exit 1
}

# Build .DATA section: EXTERN declarations for all detours
$externLines = @()
foreach ($dt in $detours) {
    $externLines += "EXTERN $($dt.ReturnVar):QWORD"
}
# CheckCall comment only on first detour
$externLines += "; EXTERN $($detours[0].CheckCallVar):QWORD           ; Uncomment if calling C++ functions from ASM"
# Conditionals only in full template for first detour
if ($fullAsm -and $detourCount -eq 1) {
    $externLines += "; EXTERN g_${featureName}_ConditionalAddr:QWORD     ; Uncomment for conditional jump address"
    $externLines += "; g_${featureName}_SomeLocalVar dd 0.0f             ; Uncomment for local data (float)"
}
$dataSection = $externLines -join "`r`n"

# Build .CODE section: PROC/ENDP for each detour
$codeBlocks = @()
foreach ($dt in $detours) {
    # Build per-detour header comments — skip "From" if same as previous
    $showFrom = (-not [string]::IsNullOrWhiteSpace($dt.FromFunc))
    if ($showFrom -and $dt.Number -gt 1) {
        $prevDt = $detours[$dt.Number - 2]
        if ($prevDt.FromFunc -eq $dt.FromFunc) { $showFrom = $false }
    }
    $procHeader = @()
    if ($showFrom) {
        $procHeader += "; From $($dt.FromFunc):"
    }
    if (-not [string]::IsNullOrWhiteSpace($dt.CEComment)) {
        $procHeader += "; $($dt.CEComment)"
    }

    $procHeaderText = ""
    if ($procHeader.Count -gt 0) {
        $procHeaderText = ($procHeader -join "`r`n") + "`r`n"
    }

    if ($fullAsm) {
        $codeBlocks += @"
$procHeaderText$($dt.DetourName) PROC

    ; --- Uncomment/use one of the following patterns ---

    ; Pattern 1: Simple detour (just jump back)
    ; jmp qword ptr [$($dt.ReturnVar)]

    ; Pattern 2: Call C++ function from ASM
    ; PushAllRegs
    ; mov rcx, rax                        ; Arg1
    ; call qword ptr [$($dt.CheckCallVar)]  ; Call C++ function
    ; PopAllRegs
    ; jmp qword ptr [$($dt.ReturnVar)]

    ; Pattern 3: Conditional jump to different code paths
    ; test rax, rax
    ; jnz ContinueExecution$($dt.Suffix)
    ; mov rax, qword ptr [g_${featureName}_ConditionalAddr]
    ; jmp rax
    ; ContinueExecution$($dt.Suffix):
    ; jmp qword ptr [$($dt.ReturnVar)]

$($dt.DetourName) ENDP
"@
    } else {
        $codeBlocks += @"
$procHeaderText$($dt.DetourName) PROC

    jmp qword ptr [$($dt.ReturnVar)]

$($dt.DetourName) ENDP
"@
    }
}

$codeSection = $codeBlocks -join "`r`n"

$asmContent = @"
INCLUDE CommonMacros.inc

.DATA
$($dataSection)

.CODE
$($codeSection)
END
"@

[System.IO.File]::WriteAllText($asmFile, $asmContent)
Write-Host "Created $asmFile" -ForegroundColor Green

# ============================================================================
# 2. Update CMakeLists.txt - add to MASM_SRC
# ============================================================================

$cmakeContent = [System.IO.File]::ReadAllText($cmakeFile)
$masmEntry = "    masm/$featureName.asm"

if ($cmakeContent -match "(?s)(set\(MASM_SRC\r?\n.*?)(\))") {
    $cmakeContent = $cmakeContent -replace "(?s)(set\(MASM_SRC\r?\n.*?)(\))", "`$1$masmEntry`r`n`$2"
    [System.IO.File]::WriteAllText($cmakeFile, $cmakeContent)
    Write-Host "Added masm/$featureName.asm to CMakeLists.txt MASM_SRC" -ForegroundColor Green
} else {
    Write-Warning "Could not find MASM_SRC in CMakeLists.txt. Please add masm/$featureName.asm manually."
}

# ============================================================================
# 3. Ask about C++ boilerplate
# ============================================================================

Write-Host ""
Write-Host "Add C++ boilerplate? [Enter = CrimsonDetours, n = skip, c = choose file]" -ForegroundColor Cyan
$addCpp = Read-Host "Choice"

if ($addCpp -eq 'n') {
    Write-Host ""
    Write-Host "Done! Checklist of remaining manual steps:" -ForegroundColor Cyan
    Write-Host "  [ ] Fill in your ASM detour logic in $featureName.asm"
    if ($fullAsm) { Write-Host "  [ ] Remove commented-out patterns you don't need from .asm" }
    Write-Host "  [ ] Add the function call in the appropriate Init/setup code"
    exit 0
}

# ============================================================================
# Helpers
# ============================================================================

function Get-ExternC-Indent {
    param([string]$content)
    $match = [regex]::Match($content, 'extern\s+"C"\s*\{')
    if (-not $match.Success) { return "`t" }
    $afterBrace = $content.Substring($match.Index + $match.Length)
    $lines = $afterBrace -split "`r?`n"
    foreach ($line in $lines) {
        $trimmed = $line.Trim()
        if (-not [string]::IsNullOrWhiteSpace($trimmed)) {
            if ($line.StartsWith("`t")) { return "`t" }
            if ($line.StartsWith("  ")) { return "  " }
            return ""
        }
    }
    return "`t"
}

function Find-ExternC-ClosingBrace {
    param([string]$content)
    $match = [regex]::Match($content, 'extern\s+"C"\s*\{')
    if (-not $match.Success) { return -1 }
    $startPos = $match.Index + $match.Length
    $depth = 1
    $pos = $startPos
    while ($depth -gt 0 -and $pos -lt $content.Length) {
        if ($content[$pos] -eq '{') { $depth++ }
        elseif ($content[$pos] -eq '}') { $depth-- }
        $pos++
    }
    return $pos - 1
}

# ============================================================================
# Determine target C++ file
# ============================================================================

$targetBaseName = "CrimsonDetours"

if ($addCpp -eq 'c') {
    $cppFiles = Get-ChildItem -Path $srcDir -Filter "Crimson*.cpp" | Sort-Object Name
    Write-Host ""
    Write-Host "Select target C++ file:" -ForegroundColor Cyan
    $i = 1
    $fileMap = @{}
    foreach ($f in $cppFiles) {
        Write-Host ("  [{0}] {1}" -f $i, $f.BaseName)
        $fileMap[$i] = $f.BaseName
        $i++
    }
    Write-Host "  [0] Create new file ($featureName.hpp / $featureName.cpp)"
    $fileChoice = Read-Host "Enter number"
    if ([int]::TryParse($fileChoice, [ref]$null) -and $fileMap[[int]$fileChoice]) {
        $targetBaseName = $fileMap[[int]$fileChoice]
    } elseif ($fileChoice -eq '0') {
        $targetBaseName = $featureName
    } else {
        Write-Warning "Invalid choice. Defaulting to CrimsonDetours."
        $targetBaseName = "CrimsonDetours"
    }
}

# ============================================================================
# 4a. Create NEW C++ files
# ============================================================================

if ($targetBaseName -eq $featureName) {
    $hppFile = Join-Path $srcDir "$featureName.hpp"
    $cppFile = Join-Path $srcDir "$featureName.cpp"

    # Build hpp: declare all detour functions in extern "C" + main toggle function
    $hppExternLines = @()
    foreach ($dt in $detours) {
        $hppExternLines += "	void $($dt.DetourName)();"
    }
    $hppExternBlock = $hppExternLines -join "`r`n"

    $hppContent = @"
#pragma once
#include <cstdint>
#include "Vars.hpp"

namespace $featureName {

extern "C" {
$($hppExternBlock)
}

void $funcName(bool enable);

}
"@
    [System.IO.File]::WriteAllText($hppFile, $hppContent)
    Write-Host "Created $hppFile" -ForegroundColor Green

    # Build cpp: extern "C" block + toggle function with all hooks
    $cppExternLines = @()
    $cppExternLines += "	// $featureName"
    foreach ($dt in $detours) {
        $cppExternLines += "	std::uint64_t $($dt.ReturnVar);"
    }
foreach ($dt in $detours) {
    $cppExternLines += "	void $($dt.DetourName)();"
}
# CheckCall comment only on first detour
$cppExternLines += "	// void* $($detours[0].CheckCallVar);  // Uncomment if calling C++ functions from ASM"
    $cppExternBlock = $cppExternLines -join "`r`n"

    # Build hook declarations and toggle calls for each detour
    $hookDeclLines = @()
    foreach ($dt in $detours) {
        $showFrom = (-not [string]::IsNullOrWhiteSpace($dt.FromFunc))
        if ($showFrom -and $dt.Number -gt 1) {
            $prevDt = $detours[$dt.Number - 2]
            if ($prevDt.FromFunc -eq $dt.FromFunc) { $showFrom = $false }
        }
        if ($showFrom) {
            $hookDeclLines += "	// From $($dt.FromFunc):"
        }
        if (-not [string]::IsNullOrWhiteSpace($dt.CEComment)) {
            $hookDeclLines += "	// $($dt.CEComment)"
        }
        $hookDeclLines += "	static std::unique_ptr<Utility::Detour_t> $($dt.HookVar) ="
        $hookDeclLines += "		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + $($dt.Offset), &$($dt.DetourName), $($dt.Size));"
        $hookDeclLines += "	$($dt.ReturnVar) = $($dt.HookVar)->GetReturnAddress();"
        if ($dt.Number -eq 1) {
            $hookDeclLines += "	// $($dt.CheckCallVar) = &SomeCppFunction;  // Uncomment if calling C++ functions"
        }
        $hookDeclLines += "	$($dt.HookVar)->Toggle(enable);"
        $hookDeclLines += ""
    }
    # Remove trailing empty line
    if ($hookDeclLines[-1] -eq "") { $hookDeclLines = $hookDeclLines[0..($hookDeclLines.Count - 2)] }
    $hookBlock = $hookDeclLines -join "`r`n"

    $cppContent = @"
#include "Core/Core.hpp"
#include "Utility/Detour.hpp"
#include "Global.hpp"
#include "$featureName.hpp"

namespace $featureName {

extern "C" {
$($cppExternBlock)
}

void $funcName(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

$($hookBlock)

	run = enable;
}

}
"@
    [System.IO.File]::WriteAllText($cppFile, $cppContent)
    Write-Host "Created $cppFile" -ForegroundColor Green

    # Add to CMakeLists.txt MAIN_SRC
    $cmakeContent = [System.IO.File]::ReadAllText($cmakeFile)
    $mainEntry1 = "    $featureName.cpp"
    $mainEntry2 = "    $featureName.hpp"
    if ($cmakeContent -match "(?s)(file\(GLOB MAIN_SRC\r?\n.*?)(\))") {
        $cmakeContent = $cmakeContent -replace "(?s)(file\(GLOB MAIN_SRC\r?\n.*?)(\))", "`$1$mainEntry1`r`n$mainEntry2`r`n`$2"
        [System.IO.File]::WriteAllText($cmakeFile, $cmakeContent)
        Write-Host "Added $featureName.cpp/.hpp to CMakeLists.txt MAIN_SRC" -ForegroundColor Green
    } else {
        Write-Warning "Could not find MAIN_SRC in CMakeLists.txt. Please add the files manually."
    }

# ============================================================================
# 4b. Add to EXISTING C++ file
# ============================================================================

} else {
    $targetHpp = Join-Path $srcDir "$targetBaseName.hpp"
    $targetCpp = Join-Path $srcDir "$targetBaseName.cpp"

    if (-not (Test-Path $targetCpp)) {
        Write-Error "File $targetCpp not found."
        exit 1
    }

    # --- Detect indentation style ---
    $cppContent = [System.IO.File]::ReadAllText($targetCpp)
    $indent = Get-ExternC-Indent $cppContent
    $usesBlankLine = $cppContent -match 'using namespace\s+\w+;\s*\r?\n\s*\r?\n\s*static'

    # --- Build extern "C" declaration block ---
    $ecNewLine = "`r`n"
    $ecPrefix = $indent

    $externCDecl = "${ecNewLine}${ecPrefix}// $featureName"
    foreach ($dt in $detours) {
        $externCDecl += "${ecNewLine}${ecPrefix}std::uint64_t $($dt.ReturnVar);"
    }
foreach ($dt in $detours) {
    $externCDecl += "${ecNewLine}${ecPrefix}void $($dt.DetourName)();"
}
# CheckCall comment only on first detour
$externCDecl += "${ecNewLine}${ecPrefix}// void* $($detours[0].CheckCallVar);  // Uncomment if calling C++ functions from ASM"
    $externCDecl += "${ecNewLine}"

    $externCPos = Find-ExternC-ClosingBrace $cppContent
    if ($externCPos -ge 0) {
        $cppContent = $cppContent.Insert($externCPos, $externCDecl)
        Write-Host "Added extern `"C`" declarations to $targetBaseName.cpp" -ForegroundColor Green
    } else {
        Write-Warning "Could not find extern `"C`" block in $targetBaseName.cpp. Please add declarations manually."
        Write-Host "Add this to the extern `"C`" block:" -ForegroundColor Yellow
        Write-Host $externCDecl
    }

    # --- Build Toggle function body with all hook declarations ---
    $toggleImpl = "`r`n`r`nvoid $funcName(bool enable) {"
    $toggleImpl += "`r`n${indent}using namespace Utility;"
    if ($usesBlankLine) { $toggleImpl += "`r`n" }
    $toggleImpl += "`r`n${indent}static bool run = false;"
    $toggleImpl += "`r`n${indent}if (run == enable) {"
    $toggleImpl += "`r`n${indent}${indent}return;"
    $toggleImpl += "`r`n${indent}}"
    $toggleImpl += "`r`n"

    foreach ($dt in $detours) {
        $showFrom = (-not [string]::IsNullOrWhiteSpace($dt.FromFunc))
        if ($showFrom -and $dt.Number -gt 1) {
            $prevDt = $detours[$dt.Number - 2]
            if ($prevDt.FromFunc -eq $dt.FromFunc) { $showFrom = $false }
        }
        $toggleImpl += "`r`n"
        if ($showFrom) {
            $toggleImpl += "${indent}// From $($dt.FromFunc):`r`n"
        }
        if (-not [string]::IsNullOrWhiteSpace($dt.CEComment)) {
            $toggleImpl += "${indent}// $($dt.CEComment)`r`n"
        }
        $hookArgs = "((uintptr_t)appBaseAddr + $($dt.Offset), &$($dt.DetourName), $($dt.Size));"
        if ($dt.Size -eq 0) {
            $hookArgs = "((uintptr_t)appBaseAddr + 0x0, &$($dt.DetourName), 0);"
        }
        $toggleImpl += "${indent}static std::unique_ptr<Utility::Detour_t> $($dt.HookVar) ="
        $toggleImpl += "`r`n${indent}${indent}std::make_unique<Detour_t>$hookArgs"
        $toggleImpl += "`r`n${indent}$($dt.ReturnVar) = $($dt.HookVar)->GetReturnAddress();"
        if ($dt.Number -eq 1) {
            $toggleImpl += "`r`n${indent}// $($dt.CheckCallVar) = &SomeCppFunction;  // Uncomment if calling C++ functions"
        }
        $toggleImpl += "`r`n${indent}$($dt.HookVar)->Toggle(enable);"
    }

    $toggleImpl += "`r`n"
    $toggleImpl += "`r`n${indent}run = enable;"
    $toggleImpl += "`r`n}"

    # Insert before the last } (closing namespace brace)
    $lastBrace = $cppContent.LastIndexOf('}')
    if ($lastBrace -ge 0) {
        $cppContent = $cppContent.Insert($lastBrace, $toggleImpl)
        Write-Host "Added $funcName function to $targetBaseName.cpp" -ForegroundColor Green
    }

    [System.IO.File]::WriteAllText($targetCpp, $cppContent)
    Write-Host "Updated $targetBaseName.cpp" -ForegroundColor Green

    # --- Modify existing .hpp ---
    if (Test-Path $targetHpp) {
        $hppContent = [System.IO.File]::ReadAllText($targetHpp)
        $hppIndent = Get-ExternC-Indent $hppContent

        # If .hpp has extern "C", add detour declarations there
        $hppExternCPos = Find-ExternC-ClosingBrace $hppContent
        if ($hppExternCPos -ge 0) {
            $detourDeclLines = @()
            $detourDeclLines += "`r`n${hppIndent}// $featureName"
            foreach ($dt in $detours) {
                $detourDeclLines += "${hppIndent}void $($dt.DetourName)();"
            }
            $detourDecl = $detourDeclLines -join "`r`n"
            $hppContent = $hppContent.Insert($hppExternCPos, $detourDecl)
        }

        # Add function declaration before closing brace of namespace
        $funcDecl = "${hppIndent}void $funcName(bool enable);`r`n"
        $lastHppBrace = $hppContent.LastIndexOf('}')
        if ($lastHppBrace -ge 0) {
            $hppContent = $hppContent.Insert($lastHppBrace, $funcDecl)
        }

        [System.IO.File]::WriteAllText($targetHpp, $hppContent)
        Write-Host "Updated $targetBaseName.hpp" -ForegroundColor Green
    } else {
        Write-Warning "Could not find $targetBaseName.hpp. Please add the $funcName declaration manually."
    }
}

# ============================================================================
# Final checklist
# ============================================================================

Write-Host ""
Write-Host "Done! Checklist of remaining manual steps:" -ForegroundColor Cyan
$hasZeroOffset = $false
$hasZeroSize = $false
foreach ($dt in $detours) {
    if ($dt.Offset -eq "0x0") { $hasZeroOffset = $true }
    if ($dt.Size -eq 0) { $hasZeroSize = $true }
}
if ($hasZeroOffset) { Write-Host "  [ ] Fill in the hook address offset(s) in C++ function" -ForegroundColor Yellow }
if ($hasZeroSize) { Write-Host "  [ ] Fill in the hook size(s) (overwritten byte count) in C++ function" -ForegroundColor Yellow }
Write-Host "  [ ] Fill in your ASM detour logic in $featureName.asm"
if ($fullAsm) { Write-Host "  [ ] Remove commented-out patterns you don't need from .asm" }
if ($targetBaseName -eq $featureName) {
    Write-Host "  [ ] Add #include `"$featureName.hpp`" where you call $funcName" -ForegroundColor Yellow
    Write-Host "  [ ] Verify $featureName.cpp/.hpp are in CMakeLists.txt MAIN_SRC" -ForegroundColor Yellow
} elseif ($targetBaseName -ne "CrimsonDetours") {
    Write-Host "  [ ] Add #include `"$targetBaseName.hpp`" where you call $funcName" -ForegroundColor Yellow
} else {
    Write-Host "  [ ] Call $funcName(enable) from the appropriate Init/setup code" -ForegroundColor Yellow
}
Write-Host "  [ ] Remove unused extern variables from .cpp if not needed" -ForegroundColor Yellow

} catch {
    Write-Host "`n============================================" -ForegroundColor Red
    Write-Host " ERROR" -ForegroundColor Red
    Write-Host "============================================" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host ""
    Write-Host "Stack trace:" -ForegroundColor DarkRed
    Write-Host $_.ScriptStackTrace -ForegroundColor DarkRed
    Write-Host "============================================" -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

# Pause so you can read the output even on success
Read-Host "`nPress Enter to exit"