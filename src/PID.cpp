#include "PID.h"
#include <cfloat>
#include <cmath>
#include <iostream>

using namespace std;

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_in, double Ki_in, double Kd_in) {
  p_error = 0;
  i_error = 0;
  d_error = 0;

  Kp = Kp_in;
  Ki = Ki_in;
  Kd = Kd_in;
  
  // Set differentials based on coefficients.  Don't want to change sensitive values like Ki by too much 
  dp = {.5*Kp,.5*Ki,.5*Kd};  
  
  // Other twiddle related vals
  steps = 0;
  total_error = 0;
  best_error = DBL_MAX;
  measure_steps = 500;
  tolerance = 0.2;
  twiddle = true;
  twiddle_index = 0;
  twiddled_up = false;
  twiddled_down = false;
}

void PID::UpdateError(double cte) {
    i_error += cte;
    d_error = cte - p_error;
    p_error = cte;
    
    // Twiddle algorithm.  Don't start until settle period is done.  
    if (twiddle && (steps > measure_steps))
    {
        total_error += pow(cte, 2);
    
        // After each measurement phase...
        if (twiddle && ((steps % measure_steps) == 0))
        {
            if ((dp[0] + dp[1] + dp[2]) < tolerance)
            {   
                // Found optimized twiddle vals based on threshold.  Stop twiddling!
                twiddle = false;
                cout << "Twiddling stopped! Final Params: " << Kp << "," << Ki << "," << Kd << endl;
                cout << "Twiddle loop took " << steps << " steps."<< endl;
                return;
            }
            
            if (total_error < best_error)
            {
                // Improvement found.  Move onto next param.  Increase dp.
                best_error = total_error;
                dp[twiddle_index] *= 1.1;
                twiddle_index = (twiddle_index + 1) % 3;
                twiddled_up = false;
                twiddled_down = false;
            }
            
            if (!twiddled_up || !twiddled_down)
            {
                // Twiddle!
                twiddle_coefficient_by_index(twiddle_index,!twiddled_up);
            }
            else
            {
                // Undo twiddle.  Neither improved error.  Reduce dp.
                twiddle_coefficient_by_index(twiddle_index,true);
                dp[twiddle_index] *= 0.9;
                twiddle_index = (twiddle_index + 1) % 3;
                twiddled_up = false;
                twiddled_down = false;
            }
            
            // Reset total error for next measurement phase.
            total_error = 0;
        }
        
        // Debug during twiddle period.
        cout << "Step: " << steps  << endl;
        cout << "DP: " << dp[0] << "," << dp[1] << "," << dp[2] << endl;
        cout << "K: " << Kp << "," << Ki << "," << Kd << endl;
    }

    steps++;
}


void PID::twiddle_coefficient_by_index(unsigned int index, bool up)
{
    // Twiddle the given coefficient up or down by the current differential amount.
    double diff = 0;
    if (up)
    {
        diff = dp[index];
        twiddled_up = true;
    }
    else
    {
        diff = -2 * dp[index]; 
        twiddled_down = true;
    }    
    
    switch(index)
    {
        case 0:
            Kp += diff;
            break;
        case 1:
            Ki += diff;
            break;
        case 2:
            Kd += diff;
            break;
        default:
            break;
    }
}


