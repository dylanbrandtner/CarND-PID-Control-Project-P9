#ifndef PID_H
#define PID_H


#include <vector>
class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */ 
  double Kp;
  double Ki;
  double Kd;
  
  /*
  * Twiddle params
  */
  double total_error;
  double best_error;
  std::vector<double> dp;
  unsigned int steps;
  bool twiddle;
  unsigned int twiddle_index;
  unsigned int measure_steps;
  bool twiddled_up;
  bool twiddled_down;
  double tolerance;


  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * Twiddle coefficient of given index up or down
  */
  void twiddle_coefficient_by_index(unsigned int index, bool up);
};

#endif /* PID_H */
