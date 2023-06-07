/*
 * Copyright (C) 2023 Coder.AN
 * Email: an.hongjun@foxmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "edge_timer.h"

Timer::Timer(std::string name, uint32_t count_n)
{
    this->name = name;
    this->count_n = count_n;
}

void Timer::start()
{
    start_time = std::chrono::system_clock::now();
}

void Timer::end()
{
    end_time = std::chrono::system_clock::now();
    once_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    count += 1;
    all_time += once_time;
    if (int(count) % count_n == 0) output();
    if (count >= count_n * 5)
    {
        count = 0;
        all_time = 0;
    }
}

void Timer::output()
{
    double avg_time = all_time / count;
    std::cout << all_time << " " << count << std::endl;
    std::cout << this->name << "[" << int(count) << "]:" << avg_time << "ms, " << 1000.0 / avg_time << "FPS" << std::endl;
}

Timer::~Timer()
{
    double avg_time = all_time / count;
    std::cout << "[Summary-" << this->name << "] Average time:" << avg_time << "ms, " << 1000.0 / avg_time << "FPS" << std::endl;
}
