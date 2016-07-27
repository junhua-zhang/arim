#include <omp.h>
//*****************************************************
// FILE CONTAINS 
//      EXTERNAL C CODE (OpenMP CPU)
//*****************************************************

//=========================================================================================================
//=========================================================================================================
// SINGLE PRECISION EXTERNAL C CODE (NEAREST TIME INSTANCE)
//=========================================================================================================
//=========================================================================================================
int c_delayAndSum_Algorithm_nearest_SP_CPU(
    const float* scanlines,                     //INPUT float complex array as [:,:,2] 2 floats [numscanlines x numsamples x 2]
    const unsigned int* tx,                     //INPUT unsigned int array [:] with size [numscanlines] transmitter
    const unsigned int* rx,                     //INPUT unsigned int array [:] with size [numscanlines] receiver
    const float* lookup_times_tx,               //INPUT float array [:,:] with size [numpoints * numelements] Flight time to transmitter from each point
    const float* lookup_times_rx,               //INPUT float array [:,:] with size [numpoints * numelements] Flight time to receiver from each point
    const float* amplitudes_tx,                 //INPUT float array [:,:] with size [numpoints * numelements] Transmitter Amplitude Array  
    const float* amplitudes_rx,                 //INPUT float array [:,:] with size [numpoints * numelements] Receiver Amplitude Array  
    float invdt,                                //INPUT float inverse time step 1/dt
    float t0,                                   //INPUT float initial time instance
    float fillvalue,                            //INPUT float fillvalue (if calculated lookup time outside sample range)
    float* result,                              //OUTPUT float array [:,2] with size [numpoints x 2 ] 2 floats representing float complex.
    int* mask,                                  //OUTPUT int array [:] with size [numpoints] representing mask (1 if valid point, 0 is no valid data at this point)                       
    int numpoints,                              //INPUT int number of points in TFM
    int numsamples,                             //INPUT int number of time samples in scanlines
    int numelements,                            //INPUT int number of elements in Probe Array
    int numscanlines,                           //INPUT int number of scanlines - which is 0.5*(numelements*numelements+numelements) for HMC, numelements*numelements for FMC
    int CHUNKSIZE)                              //INPUT int chunksize (for OpenMP CPU)

{
    /*********************************************************************************\
     Calculation of NEAREST match DELAY and SUM Algorithm 
     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
     Algorithm uses CPU. 
     
     OpenMP used for parallel processing. Number of processors is Max On Host Computer.
     
     CHUNKSIZE will be Scheduler CHUNKSIZE in this instance (see OpenMP documentation
     for information on scheduler & chunksize)

    \********************************************************************************/

    int point=0, scan=0,lookup_index=0,t_ind=0,r_ind=0,set_val=0,pixie=0,point2=0;
    float tot_real1=0,tot_imag1=0,amp_corr=0,lookup_time=0,t0forRounding=t0-0.5/invdt;
    int lbool=0;
    #pragma omp parallel firstprivate (point,scan,lookup_index,t_ind,r_ind,set_val,pixie,tot_real1,tot_imag1,amp_corr,lookup_time,point2,lbool)
    {
        #pragma omp for schedule(dynamic,CHUNKSIZE) 
        for (point=0;point<numpoints;point++)
        {
            tot_real1=0.0;
            tot_imag1=0.0;
            pixie=numelements*point;
            lbool=1;
            for (scan=0;scan<numscanlines;scan++)
            {  
                t_ind=pixie+tx[scan];
                r_ind=pixie+rx[scan];
                lookup_time = (lookup_times_tx[t_ind] + lookup_times_rx[r_ind] - t0forRounding)* invdt;
                lookup_index = (int)lookup_time;
                if (lookup_index < 0)  
                {
                    lbool=0;
                }
                else if (lookup_index >= numsamples)
                {
                    lbool=0;
                }
                else
                {
                    amp_corr = amplitudes_tx[t_ind] * amplitudes_rx[r_ind];
                    set_val=scan*numsamples+lookup_index;
                    set_val=set_val*2;
                    tot_real1 += amp_corr * scanlines[set_val]; 
                    tot_imag1 += amp_corr * scanlines[set_val+1];
                }  
            }  
            point2=point*2;
            result[point2]=tot_real1;
            result[point2+1]=tot_imag1; 
            mask[point]=lbool;
        }
    }  
    return 0;
}
