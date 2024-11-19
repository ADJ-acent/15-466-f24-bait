#pragma once
#include <glm/glm.hpp>
    
struct OBB {
    glm::vec3 center;
    glm::vec3 axes[3];
    glm::vec3 half_size;
};

bool test_axis_cross_product(const OBB& obb1, const OBB& obb2, const glm::vec3& t, float abs_r[3][3], float r[3][3], int i, int j) {
    float ra = obb1.half_size[(i + 1) % 3] * abs_r[(i + 2) % 3][j] + obb1.half_size[(i + 2) % 3] * abs_r[(i + 1) % 3][j];
    float rb = obb2.half_size[(j + 1) % 3] * abs_r[i][(j + 2) % 3] + obb2.half_size[(j + 2) % 3] * abs_r[i][(j + 1) % 3];
    float t_proj = glm::abs(t[(i + 2) % 3] * r[(i + 1) % 3][j] - t[(i + 1) % 3] * r[(i + 2) % 3][j]);
    return t_proj <= ra + rb;
}

OBB aabb_transform_to_obb(const glm::mat4x4& transform_mat, const glm::vec3 min, const glm::vec3 max) {
    // Consider four adjacent corners of the ABB
    glm::vec4 corners_aabb[4] = {
                {min.x, min.y, min.z, 1},
                {max.x, min.y, min.z, 1},
                {min.x, max.y, min.z, 1},
                {min.x, min.y, max.z, 1},
    };
    glm::vec3 corners[4];

    // Transform corners
    for (size_t corner_idx = 0; corner_idx < 4; corner_idx++) {
        glm::vec4 point = (transform_mat * corners_aabb[corner_idx]);
        corners[corner_idx] = {point.x, point.y, point.z};
    }

    // Use transformed corners to calculate center, axes and half size
    OBB obb = OBB();
    obb.axes[0] = corners[1] - corners[0];
    obb.axes[1] = corners[2] - corners[0];
    obb.axes[2] = corners[3] - corners[0];
    obb.center = corners[0] + 0.5f * (obb.axes[0] + obb.axes[1] + obb.axes[2]);
    obb.half_size = glm::vec3{ length(obb.axes[0]), length(obb.axes[1]), length(obb.axes[2]) };
    obb.axes[0] = obb.axes[0] / obb.half_size.x;
    obb.axes[1] = obb.axes[1] / obb.half_size.y;
    obb.axes[2] = obb.axes[2] / obb.half_size.z;
    obb.half_size *= 0.5f;

    return obb;
}

float abs_with_epsilon(float value, float epsilon = 1e-6f) {
    return glm::abs(value) + epsilon;
}

//test if two OBBs intersect using the Separating Axis Theorem
bool test_obb_overlap(const OBB& obb1, const OBB& obb2) {
    // Calculate the rotation matrix expressing obb2 in obb1's coordinate frame
    float r[3][3];
    float abs_r[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            r[i][j] = glm::dot(obb1.axes[i], obb2.axes[j]);
            abs_r[i][j] = abs_with_epsilon(r[i][j]);
        }
    }

    glm::vec3 t = obb2.center - obb1.center;

    t = glm::vec3(glm::dot(t, obb1.axes[0]), glm::dot(t, obb1.axes[1]), glm::dot(t, obb1.axes[2]));

    for (int i = 0; i < 3; ++i) {
        float ra = obb1.half_size[i];
        float rb = obb2.half_size[0] * abs_r[i][0] + obb2.half_size[1] * abs_r[i][1] + obb2.half_size[2] * abs_r[i][2];
        if (glm::abs(t[i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; ++i) {
        float ra = obb1.half_size[0] * abs_r[0][i] + obb1.half_size[1] * abs_r[1][i] + obb1.half_size[2] * abs_r[2][i];
        float rb = obb2.half_size[i];
        if (glm::abs(t[0] * r[0][i] + t[1] * r[1][i] + t[2] * r[2][i]) > ra + rb) return false;
    }

    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 0, 0)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 0, 1)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 0, 2)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 1, 0)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 1, 1)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 1, 2)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 2, 0)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 2, 1)) return false;
    if (!test_axis_cross_product(obb1, obb2, t, abs_r, r, 2, 2)) return false;

    return true;
}

bool test_mesh_obb_overlap(const glm::vec3& mesh1_min, const glm::vec3& mesh1_max, const glm::mat4x4 world_from_local1, const glm::vec3& mesh2_min, const glm::vec3& mesh2_max, glm::mat4x4 world_from_local2) {
    OBB obb1 = aabb_transform_to_obb(world_from_local1, mesh1_min, mesh1_max);
    OBB obb2 = aabb_transform_to_obb(world_from_local2, mesh2_min, mesh2_max);
    return test_obb_overlap(obb1, obb2);
}