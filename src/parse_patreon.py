#!/usr/bin/env python3
"""
Parses patreonlist.csv and generates patrons.json and updates Patreon.hpp
with real Patreon supporter names and tiers.

Usage: python parse_patreon.py
"""

import csv
import json
import os
import re

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

CSV_PATH = os.path.join(SCRIPT_DIR, "patreonlist.csv")
JSON_PATH = os.path.join(SCRIPT_DIR, "patrons.json")
HPP_PATH = os.path.join(SCRIPT_DIR, "Patreon.hpp")

# Tier name mapping from CSV -> internal keys
TIER_MAP = {
    "Devil Trigger Tier": "DT",
    "Sin Devil Trigger Tier": "SDT",
    "Legendary Dark Knight Tier": "LDK",
}

# Read and parse CSV
patrons = {"DT": [], "SDT": [], "LDK": []}

with open(CSV_PATH, mode="r", encoding="utf-8-sig") as f:
    reader = csv.DictReader(f)
    for row in reader:
        status = row.get("Patron Status", "").strip()
        tier_raw = row.get("Tier", "").strip()
        name = row.get("Name", "").strip()

        # Only include active patrons
        if status.lower() != "active patron":
            continue

        # Map tier name
        tier_key = TIER_MAP.get(tier_raw)
        if tier_key is None:
            print(f"  [skip] Unknown tier '{tier_raw}' for {name}")
            continue

        if not name:
            print(f"  [skip] Empty name for tier {tier_key}")
            continue

        patrons[tier_key].append(name)

# Sort each tier alphabetically for consistency
for key in patrons:
    patrons[key].sort()

# ── Write patrons.json ──
tier_display = {
    "DT": "Devil Trigger Tier",
    "SDT": "Sin Devil Trigger Tier",
    "LDK": "Legendary Dark Knight Tier",
}

json_data = {
    "tierNames": tier_display,
    "patrons": patrons,
}

with open(JSON_PATH, mode="w", encoding="utf-8", newline="\n") as f:
    json.dump(json_data, f, indent="\t", ensure_ascii=False)
    f.write("\n")

print(f"✓ Wrote {JSON_PATH}")
for tier, names in patrons.items():
    print(f"   {tier}: {len(names)} patrons")

# ── Generate C++ code for Patreon.hpp ──
def gen_cpp_vector(var_name, names):
    if not names:
        return f"\t{var_name}.clear();"
    lines = [f"\t{var_name}.reserve({len(names)});"]
    for n in names:
        # Escape any backslashes or quotes in names
        safe = n.replace("\\", "\\\\").replace('"', '\\"')
        lines.append(f'\t{var_name}.push_back("{safe}");')
    return "\n".join(lines)

cpp_tier_names = {
    "DT": "Devil Trigger Tier",
    "SDT": "Sin Devil Trigger Tier",
    "LDK": "Legendary Dark Knight Tier",
}

cpp_code = f"""\t// Tier display names
\tg_UIContext.TierNames[(size_t)PatreonTiers_t::DT]  = "{cpp_tier_names['DT']}";
\tg_UIContext.TierNames[(size_t)PatreonTiers_t::SDT] = "{cpp_tier_names['SDT']}";
\tg_UIContext.TierNames[(size_t)PatreonTiers_t::LDK] = "{cpp_tier_names['LDK']}";

\t// ── DT ──
{gen_cpp_vector("g_UIContext.PatronsDT", patrons["DT"])}

\t// ── SDT ──
{gen_cpp_vector("g_UIContext.PatronsSDT", patrons["SDT"])}

\t// ── LDK ──
{gen_cpp_vector("g_UIContext.PatronsLDK", patrons["LDK"])}
"""

print("\n── Paste this into Patreon.hpp `LoadDefaultPatrons()` ──")
print(cpp_code)
