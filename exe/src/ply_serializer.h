/*
    ply_serializer.h: Helper class to serialize the application state to a .PLY file

    This file is part of the implementation of

        Instant Field-Aligned Meshes
        Wenzel Jakob, Daniele Panozzo, Marco Tarini, and Olga Sorkine-Hornung
        In ACM Transactions on Graphics (Proc. SIGGRAPH Asia 2015)

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include "serializer.h"
#include "common.h"

#include <map>
#include <set>
#include <stack>

class PlySerializer : public Serializer {
public:
    PlySerializer();
    PlySerializer(const std::string &filename, bool compatibilityMode = false,
               const ProgressCallback &progress = ProgressCallback());
    virtual ~PlySerializer() = default;

    void write(const std::string &filename,
               const ProgressCallback &progress = ProgressCallback());

    static bool isSerializedFile(const std::string &filename);
};
