/* The DC Analysis of All JFET Transistor Configurations

I've started this project for making easier the transistor and 
related operations. The important point is that all transistors are
'npn' types. But, can be derivated easily 'pnp' implementations. 
This package contains mostly used configuration and some special
connections. Another important point is that in AC analysis of
transistors, algorithms use 'JFET small-signal' transistor model.
In specially, Bbecause JFET and D-MOSFET analzes are same things, 
I put these analyzes into 'FET.h' source code.

Resource: Electronic Devices and Circuit Theory by Robert L. 
Boylestad and Louis Nashelsky
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "FET.h"

/* Main method that will display the all implemnetations */
int main(void) {
   // 'analysis' argument represents the type of analysis.
   char analysis[2];
   // 'transistor' argument represents type of transistor.
   char transistor[2];
   // These arguments represent the values of a transistor. 
   float Vdd, Vgg, Rd, Idss, Vp, Rs, Vss, Rg1, Rg2, rd, Vgs;

   puts("--------------------------------------------");
   puts("  WELLCOME TO ANALYSIS OF JFET TRANSISTORS  ");
   puts("--------------------------------------------");
   puts("ANALYSIS:");
   puts("--> 'dc' for alternative current");
   puts("--> 'ac' for direct current");
   printf("Analysis mode: "); scanf("%s", &analysis);
   puts("--------------------------------------------");

   // For DC Analysis:
   if (strcmp(analysis, "dc") == 0) {
      // Display and get the transistors and its types.
      _display_transistors_("dc", &transistor);
      // For Fixed-Bias Configuration:
      if (strcmp(transistor, "fb") == 0) {
         // Get inputs of fixed-bias config.
         _fixed_bias_inputs_("dc", &Vdd, &Vgg, 1, &Rd, &Idss, &Vp, 1);
         // Calculate the results of fixed-bias config.
         fixed_bias("dc", Vdd, Vgg, 1, Rd, Idss, Vp, 1);
         // Display the results of fixed-bias config.
         _display_dc_results_();
      }
      // For Self-Bias Configuration:
      else if (strcmp(transistor, "sb") == 0) {
         // Get inputs of self-bias config.
         _self_bias_inputs_("dc", &Vdd, 1, &Rd, &Rs, &Idss, &Vp, 1);
         // Calculate the results of self-bias config.
         self_bias("dc", Vdd, 1, Rd, Rs, Idss, Vp, 1);
         // Display the results of fixed-bias config.
         _display_dc_results_();
      }
      // For Voltage-Divider Configuration:
      else if (strcmp(transistor, "vd") == 0) {
         // Get inputs of voltage-divider config.
         _voltage_divider_inputs_("dc", &Vdd, &Rg1, &Rg2, &Rd, &Rs, 
                                  &Idss, &Vp, 1);
         // Calculate the results of voltage-divider config.
         voltage_divider("dc", Vdd, Rg1, Rg2, Rd, Rs, Idss, Vp, 1);
         // Display the results of voltage-divider config.
         _display_dc_results_();
      }
      // For Common-Gate Configuration:
      else if (strcmp(transistor, "cg") == 0) {
         // Get inputs of common-gate config.
         _common_gate_inputs_("dc", &Vdd, &Vss, &Rd, &Rs, &Idss, 
                              &Vp, 1);
         // Calculate the results of common-gate config.
         common_gate("dc", Vdd, Vss, Rd, Rs, Idss, Vp, 1);
         // Display the results of common-gate config.
         _display_dc_results_();
      }
      else puts("Can not found that transistor !!!");
   }
   // For AC Analysis:
   else if (strcmp(analysis, "ac") == 0) {
      // Display and get the transistors and types.
      _display_transistors_("ac", &transistor);
      // For Fixed-Bias Configuration:
      if (strcmp(transistor, "fb") == 0) {
         // Get inputs of fixed-bias config.
         _fixed_bias_inputs_("ac", &Vdd, &Vgg, &Rg1, &Rd, &Idss, 
                             &Vp, &rd);
         // Calculate the results of fixed-bias config.
         fixed_bias("ac", Vdd, Vgg, Rg1, Rd, Idss, Vp, rd);
         // Display the results of fixed-bias config.
         _display_ac_results_();
      }
      // For Self-Bias Configuration:
      else if (strcmp(transistor, "sb") == 0) {
         // Get inputs of self-bias config.
         _self_bias_inputs_("ac", &Vdd, &Rg1, &Rd, &Rs, &Idss, 
                            &Vp, &rd);
         // Calculate the results of self-bias config.
         self_bias("ac", Vdd, Rg1, Rd, Rs, Idss, Vp, rd);
         // Display the results of self-bias config.
         _display_ac_results_();
      }
      // For Voltage-Divider Configuration:
      else if (strcmp(transistor, "vd") == 0) {
         // Get inputs of voltage-divider config.
         _voltage_divider_inputs_("ac", &Vdd, &Rg1, &Rg2, &Rd, &Rs, 
                                  &Idss, &Vp, &rd);
         // Calculate the results of voltage-divider config.
         voltage_divider("ac", Vdd, Rg1, Rg2, Rd, Rs, Idss, Vp, rd);
         // Display the results of voltage-divider config.
         _display_ac_results_();
      }
      // For Common-Gate Configuration:
      else if (strcmp(transistor, "cg") == 0) {
         // Get inputs of common-gate config.
         _common_gate_inputs_("ac", &Vdd, &Vss, &Rd, &Rs, &Idss, 
                              &Vp, &rd);
         // Calculate the results of common-gate config.
         common_gate("ac", Vdd, Vss, Rd, Rs, Idss, Vp, rd);
         // Display the results of common-gate config.
         _display_ac_results_();
      }
      // For Self-Follower Configuration:
      else if (strcmp(transistor, "sf") == 0) {
         // Get inputs of self-follower config.
         _source_follower_inputs_("ac", &Vdd, &Vgs, &Rg1, &Rs, 
                                  &Idss, &Vp, &rd);
         // Calculate the results of self-follower config.
         source_follower("ac", Vdd, Vgs, Rg1, Rs, Idss, Vp, rd);
         // Display the results of self-follower config.
         _display_ac_results_();
      }
      else puts("Can not found that transistor !!!");
   }
   else puts("Can not found that analysis !!!");

   return 0;
}