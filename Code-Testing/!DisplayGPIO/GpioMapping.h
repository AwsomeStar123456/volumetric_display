//This file maps a 2D LED array to GPIO assignments.
//LED Array: IS31FL3731
//First value is VCC second is Ground
//gpiomap[y][x][0] = VCC
//gpiomap[y][x][1] = GND

//gpiomap_flipped[x][y][0] = VCC
//gpiomap_flipped[x][y][1] = GND

int gpiomap_flipped[9][16][2] = {
    {{0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {9, 27}, {10, 27}, {11, 27}, {12, 27}, {13, 27}, {14, 27}, {15, 27}, {26, 27}},
    {{0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {8, 7}, {9, 26}, {10, 26}, {11, 26}, {12, 26}, {13, 26}, {14, 26}, {15, 26}, {27, 26}},
    {{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {7, 6}, {8, 6}, {9, 15}, {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15}, {26, 15}, {27, 15}},
    {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 14}, {10, 14}, {11, 14}, {12, 14}, {13, 14}, {15, 14}, {26, 14}, {27, 14}},
    {{0, 4}, {1, 4}, {2, 4}, {3, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 13}, {10, 13}, {11, 13}, {12, 13}, {14, 13}, {15, 13}, {26, 13}, {27, 13}},
    {{0, 3}, {1, 3}, {2, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 12}, {10, 12}, {11, 12}, {13, 12}, {14, 12}, {15, 12}, {26, 12}, {27, 12}},
    {{0, 2}, {1, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 11}, {10, 11}, {12, 11}, {13, 11}, {14, 11}, {15, 11}, {26, 11}, {27, 11}},
    {{0, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 10}, {11, 10}, {12, 10}, {13, 10}, {14, 10}, {15, 10}, {26, 10}, {27, 10}},
    {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {10, 9}, {11,  9}, {12,  9}, {13,  9}, {14,  9}, {15, 9 }, {26,  9}, {27,  9}}
};




// int gpiomap[16][9][2] = {
//     //Matrix A
//     {{0, 8}, {0, 7}, {0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {1, 0}},
//     {{1, 8}, {1, 7}, {1, 6}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {2, 1}, {2, 0}},
//     {{2, 8}, {2, 7}, {2, 6}, {2, 5}, {2, 4}, {2, 3}, {3, 2}, {3, 1}, {3, 0}},
//     {{3, 8}, {3, 7}, {3, 6}, {3, 5}, {3, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}},
//     {{4, 8}, {4, 7}, {4, 6}, {4, 5}, {5, 4}, {5, 3}, {5, 2}, {5, 1}, {5, 0}},
//     {{5, 8}, {5, 7}, {5, 6}, {6, 5}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}},
//     {{6, 8}, {6, 7}, {7, 6}, {7, 5}, {7, 4}, {7, 3}, {7, 2}, {7, 1}, {7, 0}},
//     {{7, 8}, {8, 7}, {8, 6}, {8, 5}, {8, 4}, {8, 3}, {8, 2}, {8, 1}, {8, 0}},
//     //Matrix B
//     {{ 9, 27}, { 9, 26}, { 9, 15}, { 9, 14}, { 9, 13}, { 9, 12}, { 9, 11}, { 9, 10}, {10, 9}},
//     {{10, 27}, {10, 26}, {10, 15}, {10, 14}, {10, 13}, {10, 12}, {10, 11}, {11, 10}, {11, 9}},
//     {{11, 27}, {11, 26}, {11, 15}, {11, 14}, {11, 13}, {11, 12}, {12, 11}, {12, 10}, {12, 9}},
//     {{12, 27}, {12, 26}, {12, 15}, {12, 14}, {12, 13}, {13, 12}, {13, 11}, {13, 10}, {13, 9}},
//     {{13, 27}, {13, 26}, {13, 15}, {13, 14}, {14, 13}, {14, 12}, {14, 11}, {14, 10}, {14, 9}},
//     {{14, 27}, {14, 26}, {14, 15}, {15, 14}, {15, 13}, {15, 12}, {15, 11}, {15, 10}, {15, 9}},
//     {{15, 27}, {15, 26}, {26, 15}, {26, 14}, {26, 13}, {26, 12}, {26, 11}, {26, 10}, {26, 9}},
//     {{26, 27}, {27, 26}, {27, 15}, {27, 14}, {27, 13}, {27, 12}, {27, 11}, {27, 10}, {27, 9}}
// };