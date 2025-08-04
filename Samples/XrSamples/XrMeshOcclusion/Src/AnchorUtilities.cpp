/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/************************************************************************************

Filename  : AnchorUtilities.cpp
Content   : Utility functions for XrSceneSharing
Created   :
Authors   :

Copyright : Copyright (c) Meta Platforms, Inc. and its affiliates. All rights reserved.

*************************************************************************************/

#include "AnchorUtilities.h"

std::string bin2hex(const uint8_t* src, uint32_t size) {
    std::string res;
    res.reserve(size * 2);
    const char hex[] = "0123456789ABCDEF";
    for (uint32_t i = 0; i < size; ++i) {
        uint8_t c = src[i];
        res += hex[c >> 4];
        res += hex[c & 0xf];
    }
    return res;
}

std::string uuidToHexString(const XrUuidEXT& uuid) {
    return bin2hex(reinterpret_cast<const uint8_t*>(uuid.data), XR_UUID_SIZE_EXT);
}

bool hexStringToUuid(const std::string& hex, XrUuidEXT& uuid) {
    if (hex.length() != XR_UUID_SIZE_EXT * 2) {
        return false;
    }
    for (uint32_t i = 0, k = 0; i < XR_UUID_SIZE_EXT; i++, k += 2) {
        std::string byteStr = hex.substr(k, 2);
        uuid.data[i] = (uint8_t)stol(byteStr, nullptr, 16);
    }
    return true;
}

bool isValid(const XrUuidEXT& uuid) {
    for (int i = 0; i < XR_UUID_SIZE_EXT; ++i) {
        if (uuid.data[i] > 0) {
            return true;
        }
    }
    return false;
}
