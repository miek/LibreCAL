/*
 * touchstone.hpp
 *
 *  Created on: Sep 5, 2022
 *      Author: jan
 */

#ifndef SRC_TOUCHSTONE_HPP_
#define SRC_TOUCHSTONE_HPP_

#include <cstdint>

namespace Touchstone {

uint32_t GetPointNum(const char *folder, const char *filename);
bool StartNewFile(const char *folder, const char *filename);
bool AddPoint(double frequency, double *values, uint8_t num_values);
bool FinishFile();
int GetPoint(const char *folder, const char *filename, uint32_t point, double *values);

};

#endif /* SRC_TOUCHSTONE_HPP_ */
