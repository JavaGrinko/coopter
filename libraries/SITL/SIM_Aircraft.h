/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
  parent class for aircraft simulators
*/

#ifndef _SIM_AIRCRAFT_H
#define _SIM_AIRCRAFT_H

#include "SITL.h"
#include <AP_Common/AP_Common.h>
#include <AP_Math/AP_Math.h>

/*
  parent class for all simulator types
 */
class Aircraft
{
public:
    Aircraft(const char *home_str, const char *frame_str);

    /*
      structure passed in giving servo positions as PWM values in
      microseconds
     */
    struct sitl_input {
        uint16_t servos[16];
        struct {
            float speed;      // m/s
            float direction;  // degrees 0..360
            float turbulence;
        } wind;
    };

    /*
      set simulation speedup
     */
    void set_speedup(float speedup);

    /*
      set instance number
     */
    void set_instance(uint8_t _instance) {
        instance = _instance;
    }

    /*
      set directory for additional files such as aircraft models
     */
    void set_autotest_dir(const char *_autotest_dir) {
        autotest_dir = _autotest_dir;
    }

    /*
      step the FDM by one time step
     */
    virtual void update(const struct sitl_input &input) = 0;

    /* fill a sitl_fdm structure from the simulator state */
    void fill_fdm(struct sitl_fdm &fdm) const;

    int sock;
protected:
    Location home;
    Location location;

    float ground_level;
    float frame_height;
    Matrix3f dcm;  // rotation matrix, APM conventions, from body to earth
    Vector3f gyro; // rad/s
    Vector3f velocity_ef; // m/s, earth frame
    Vector3f position; // meters, NED from origin
    float mass; // kg
    Vector3f accel_body; // m/s/s NED, body frame
    float airspeed; // m/s, apparent airspeed

    uint64_t time_now_us;

    const float gyro_noise;
    const float accel_noise;
    float rate_hz;
    float achieved_rate_hz;
    float target_speedup;
    uint64_t frame_time_us;
    float scaled_frame_time_us;
    uint64_t last_wall_time_us;
    uint8_t instance;
    const char *autotest_dir;
    const char *frame;

    bool on_ground(const Vector3f &pos) const;

    /* update location from position */
    void update_position(void);

    /* rotate to the given yaw */
    void set_yaw_degrees(float yaw_degrees);

    /* advance time by deltat in seconds */
    void time_advance(float deltat);

    /* setup the frame step time */
    void setup_frame_time(float rate, float speedup);

    /* adjust frame_time calculation */
    void adjust_frame_time(float rate);

    /* try to synchronise simulation time with wall clock time, taking
       into account desired speedup */
    void sync_frame_time(void);

    /* add noise based on throttle level (from 0..1) */
    void add_noise(float throttle);

    /* return wall clock time in microseconds since 1970 */
    uint64_t get_wall_time_us(void) const;

    /* return normal distribution random numbers */
    double rand_normal(double mean, double stddev);

private:
    uint64_t last_time_us = 0;
    uint32_t frame_counter = 0;
    const uint32_t min_sleep_time;
};

#endif // _SIM_AIRCRAFT_H

