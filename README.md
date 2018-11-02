# PID Controller

[//]: # (Image References)
[image1]: ./doc/intro.png  "intro"
[image2]: ./doc/Filter_algo.png  "algo"
[image3]: ./doc/result.png  "result"

## Project Goal
The goal of this project is to create a PID controller that takes in a "Cross Track Error" value and outputs a steering angle.  

## [Rubric Points](https://review.udacity.com/#!/rubrics/824/view)

Here I will consider the rubric points individually.  

## Running the Code
This project uses the Term 2 Simulator which can be downloaded [here](https://github.com/udacity/self-driving-car-sim/releases)

The main program can be built and run by doing the following from the project's top level directory.

1. mkdir build && cd build
2. cmake .. && make
3. ./pid

## Implementation

The PID procedure follows what was taught in the lessons.  In general, this includes two parts: The PID Algorithm itself, and parameter optimization. 

### PID Algorithm 

The PID Algorithm looks like this: `angle = -tau_p * cte - tau_d * diff_cte - tau_i * int_cte`.  Terms are described below:

|  Term    | Description              |
|:---------:|:-----------------------:|
| angle    | Output Steering Angle    |
| tau_p    | Proportional Coefficient |
| cte      | Cross Track Error        |
| tau_d    | Differential Coefficient |
| diff_cte | Differential CTE         |
| tau_i    | Integral Coefficient     |
| int_cte  | Integral CTE             |

This is implemented in the `main()` function.  The coefficient values are discussed in the section below, but the diff_cte and int_cte values are based on the incoming CTE on each step.  diff_cte is the difference between the current and last CTE value, and int_cte is the sum of all previous CTE values.  Those values are updated in the `UpdateError()` function of the `PID` class.

### Parameter tuning

The above algorithm is simple.  The complex part of this task is how to chose appropriate values for the three coefficients.  This was done using a method called "twiddle".  The twiddle implementation can be found in the second half of the `UpdateError()` function of the `PID` class.  More discussion on this can be found in the "Choosing hyperparameters" section below.  The final coefficients arrived at by the twiddle algorithm were as follows:

| Coefficient  | Value |
|:------------:|:-----:|
| Proportional |       |
| Differential |       |
| Integral     |       |


## Reflection

### Effects of P,I,D components 

All three elements of the PID algorithm have equal weight by default.  However, the 3 coefficients change this weight.  The "I" or "Integral" component is the sum of all previous CTE values, so it obviously has the largest impact.  Thus, the coefficient needs to be quite small to avoid this value overwhelming the algorithm.  In some of my initial testing, I made this value far too high by mistake, and the car was violently steering from left to right.  The "P" or "Proportional" component is the CTE value itself, so it has the second highest impact.  This is the most reactive of the components as it is based solely on the current reading.  It is a direct measure of the how far the car is from the desired path.  The "D" or "Differential" component is the difference from the previous CTE value, so it is typically quite small.  To have any impact, the coefficient here must be significantly larger than the others.  This component acts as a "smoothing" parameter to reduce the turn angle as the CTE decreases (i.e. as the car reaches its destination path).  

### Choosing hyperparameters

Based on the above knowledge of their impacts, and using the example values from the course , I started with P, I, and D coefficients of 0.2, 0.004, and 3.0 respectively. Then I used the "twiddle" algorithm in the class to tune these based on measurements in the drive.  The initial values were enough to have the car drive reasonably well, and I turned the throttle value down to 0.1 during the twiddle period to avoid driving too quickly with sub-optimal parameters.  Here's a [link to a video recording](./initial_coeff.mp4) of the car driving with the initial coefficient values mentioned above.

After this, I had to choose a period of time to measure total error in for the twiddle algorithm.  I choose 500 steps for this as this covered a reasonable span of the track in my testing.  For every 500 steps, the twiddle algorithm either increased or decreased the coefficient by a predefined differential for that coefficient.  This differential started with half the value of the initial coefficient. (I initially used "1" for each of these differentials, but the impact this had on the "I" component of the algorithm was so large, that each time this was done the car would leave the track and not be able to correct).  If either change represented an improvement in the overall measured error, the coefficient would be set to that value and the differential would be made smaller.  If neither change improved the measured error, the coefficient would be reset to its value before the change, and the differential would grow larger. This process was repeated until a certain threshold was met.  As in the course materials, I chose of value of .2 to be the threshold that all the differentials should sum to.  In the end, the final parameters chosen by the algorithm are noted in the "Parameter tuning" section above.  It took the algorithm ### steps to arrive at these values. 
 
## Results

Here's a [link to a video recording of my final result](./project_recording.mp4).  

The rubric specifies "No tire may leave the drivable portion of the track surface. The car may not pop up onto ledges or roll over any surfaces that would otherwise be considered unsafe (if humans were in the vehicle)."  I believe this recording meets that specification.  

NOTE: This video was recorded after the twiddle algorithm completed as described above.  To more quickly arrive at this result, one could change the initial coefficients to those described in the "Parameter tuning" section.  Do this by updating the `pid.Init()` values in the `main()` function, and set the initial value of the `twiddle` boolean to `false`.  In other words, uncomment the code at lines 39 and 40 of main.cpp, rebuild, and re-run.  

