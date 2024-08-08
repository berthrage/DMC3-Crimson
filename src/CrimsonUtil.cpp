// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#include "DMC3Input.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"
#include <cstdio>
#include "Core/Macros.h"

int GetPlayerIndexFromAddr(uintptr_t playerPtr) {
    int playerIndexFound = 10; // if none is found then it'll return 10.

    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        if (playerPtr == crimsonPlayer[playerIndex].playerPtr || playerPtr == crimsonPlayer[playerIndex].clonePtr) {
            playerIndexFound = playerIndex;
        }
    }

    return playerIndexFound;
}

bool IsAddressValidAndWritable(uintptr_t address) {
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
        // Check if the address is committed memory and is writable
        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READWRITE ||
                mbi.Protect == PAGE_WRITECOPY ||
                mbi.Protect == PAGE_EXECUTE_READWRITE ||
                mbi.Protect == PAGE_EXECUTE_WRITECOPY)) {
            return true;
        }
    }
    return false;
}

