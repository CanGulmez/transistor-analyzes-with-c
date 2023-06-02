/* The DC Analysis of All MOSFET Transistor Configurations

I've started this project for making easier the transistor and 
related operations. The important point is that all transistors are
'npn' types. But, can be derivated easily 'pnp' implementations. 
This package contains mostly used configuration and some special
connections. In specially, this source code contains e-type mosfet
analyzes. Because, JFET and D-MOSFET analyzes are same things. 
Common analyzes of two one can be found 'FET.h' source code. 

Resource: Electronic Devices and Circuit Theory by Robert L. 
Boylestad and Louis Nashelsky
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "FET.h"

// Results of DC Analysis
struct DCMOSFET {
   float k; // k constant
   float Id; // drain current
   float Vgs; // gate-source voltage
   float Vds; // drain-source voltage
};
// Results of AC Analysis
struct ACMOSFET {
   float gm; // transconductance factor
   float Zi; // input impedance
   float Zo; // output impedance
   float Av; // voltage gain
   char* phase; // phase relationship
};

// For all DC configuration:
struct DCMOSFET DCMOSFET;
// For all AC configuration:
struct ACMOSFET ACMOSFET;

/* The DC and AC Analysis of Drain-Feedback Configuration */
void m_drain_feedback(char* analysis, float Vdd, float Rg, float Rd, 
                    float Idon, float Vgson, float Vgsth, float rd) {
   // Check if the parameters are correct.
   assert (Rg > 0 && Rd > 0 && rd > 0);
   // These results are required for both analysis type.
   float k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   float a = Rd * Rd * k;
   float b = 2 * k * Rd * (Vgsth - Vdd) - 1;
   float c = k * (Vgsth -Vdd) * (Vgsth - Vdd);
   // Can be used dicriminant for finding right 'Id'.
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vdd - Id * Rd; // gate-source voltage
   // DC Analysis:
   float Vds = Vgs; // drain-source voltage
   // AC Analysis:
   float gm = 2 * k * (Vgs - Vgsth); // gm factor
   float Zi1 = Rg + _parallel_(rd, Rd);
   float Zi2 = 1 + gm * _parallel_(rd, Rd); 
   float Zi = Zi1 / Zi2; // input impedance
   float Zo = _parallel_(Rg, _parallel_(rd, Rd)); // output impedance
   // voltage gain
   float Av = -1 * gm * _parallel_(Rg, _parallel_(rd, Rd)); 
   // Save the results.
   if (strcmp(analysis, "ac")) {
      DCMOSFET.k = k; DCMOSFET.Id = Id; DCMOSFET.Vgs = Vgs;
      DCMOSFET.Vds = Vds;
   } else {
      ACMOSFET.gm = gm; ACMOSFET.Zi = Zi; ACMOSFET.Zo = Zo;
      ACMOSFET.Av = Av; ACMOSFET.phase = "Out of phase";
   }
}

/* The DC and AC Analysis of Voltage-Divider Configuration */
void m_voltage_divider(char* analysis, float Vdd, float Rg1, 
                       float Rg2, float Rd, float Rs, float Idon, 
                       float Vgson, float Vgsth, float rd) {
   // Check if the parameters are correct.
   assert (Rg1 > 0 && Rg1 > 0 && Rd > 0 && Rs > 0 && rd > 0);
   // These results are required for both analysis type.
   float k = Idon / ((Vgson - Vgsth) * (Vgson - Vgsth));
   float Vg = Rg2 * Vdd / (Rg1 + Rg2);
   float a = Rs * Rs * k;
   float b = -2 * k * Rs * (Vg - Vgsth) - 1;
   float c = k * (Vgsth - Vg) * (Vgsth - Vg);
   // Can be used dicriminant for finding right 'Id'.
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vg - Id * Rs; // gate-source voltage
   // DC Analysis:
   float Vds = Vdd - Id * (Rs + Rd);
   // AC Analysis:
   float gm = 2 * k * (Vgs - Vgsth); // gm factor
   float Zi = _parallel_(Rg1, Rg2); // input impedance
   float Zo = _parallel_(rd, Rd); // output impedance
   float Av = -1 * gm * Zo; // voltage gain
   // Save the results.
   if (strcmp(analysis, "ac")) {
      DCMOSFET.k = k; DCMOSFET.Id = Id; DCMOSFET.Vgs = Vgs;
      DCMOSFET.Vds = Vds;
   } else {
      ACMOSFET.gm = gm; ACMOSFET.Zi = Zi; ACMOSFET.Zo = Zo;
      ACMOSFET.Av = Av; ACMOSFET.phase = "Out of phase";
   }
}

void _m_display_transistor(char* analysis, char* transistor) {
   // Display the all transistor configurations.
   puts("TRANSISTOR:");
   if (strcmp(analysis, "ac"))
   puts("--> 'fb' for feedback biasing config.");
   else
   puts("--> 'df' for drain-feedback config.");
   puts("--> 'vd' for voltage-divider config.");
   printf("Transistor type: "); scanf("%s", &(*transistor));
   puts("----------------------------------------------");
}

void _m_drain_feedback_inputs_(char* analysis, float* Vdd, float* Rg,
                               float* Rd, float* Idon, float* Vgson, 
                               float* Vgsth, float* rd) {
   // Get inputs of feedback biasing config.
   puts("PARAMETERS: ");
   printf("Vdd (V): "); scanf("%f", &(*Vdd));
   if (strcmp(analysis, "ac")) {
      printf("Rg (ohm): "); scanf("%f", &(*Rg)); 
   }
   else {
      printf("Rf (ohm): "); scanf("%f", &(*Rg));
   }
   printf("Rd (ohm): "); scanf("%f", &(*Rd));
   printf("Id(on) (A): "); scanf("%f", &(*Idon));
   printf("Vgs(on) (V): "); scanf("%f", &(*Vgson));
   printf("Vgs(th) (V): "); scanf("%f", &(*Vgsth));
   if (strcmp(analysis, "dc")) {
      printf("rd (ohm): "); scanf("%f", &(*rd));
   }
   puts("Calculating results ...");
   puts("----------------------------------------------");
}

void _m_voltage_divider_inputs_(char* analysis, float* Vdd, 
                                float* Rg1, float* Rg2, float* Rd, 
                                float* Rs, float* Idon, float* Vgson, 
                                float* Vgsth, float* rd) {
   // Get inputs of voltage-divider config.
   puts("PARAMETERS: ");
   printf("Vdd (V): "); scanf("%f", &(*Vdd));
   printf("Upper Rg (ohm): "); scanf("%f", &(*Rg1)); 
   printf("Lower Rg (ohm): "); scanf("%f", &(*Rg2));
   printf("Rd (ohm): "); scanf("%f", &(*Rd));
   printf("Rs (ohm): "); scanf("%f", &(*Rs));
   printf("Id(on) (A): "); scanf("%f", &(*Idon));
   printf("Vgs(on) (V): "); scanf("%f", &(*Vgson));
   printf("Vgs(th) (V): "); scanf("%f", &(*Vgsth));
   if (strcmp(analysis, "dc")) {
      printf("rd (ohm): "); scanf("%f", &(*rd));
   }
   puts("Calculating results ...");
   puts("----------------------------------------------");
}

int main(void) {
   // 'mosfet' argument represents type of mosfet transistor.
   char mosfet;
   // 'analysis' argument represents the type of analysis.
   char analysis[2];
   // 'transistor' argument represents type of transistor.
   char transistor[2];
   // These arguments represent the values of a transistor. 
   float Vdd, Vgg, Rd, Idss, Vp, Rs, Vss, Rg1, Rg2, rd, Vgs;
   float Idon, Vgson, Vgsth;

   puts("----------------------------------------------");
   puts("  WELLCOME TO ANALYSIS OF MOSFET TRANSISTORS  ");
   puts("----------------------------------------------");
   puts("MOSFET: ");
   puts("--> 'd' for deplation-type mosfet");
   puts("--> 'e' for enhancment-type mosfet");
   printf("Mosfet type: "); scanf("%c", &mosfet);
   puts("----------------------------------------------");
   puts("ANALYSIS:");
   puts("--> 'dc' for alternative current");
   puts("--> 'ac' for direct current");
   printf("Analysis mode: "); scanf("%s", &analysis);
   puts("----------------------------------------------"); 

   // For DC Analysis of Deplation-Type MOSFET:
   if (strcmp(analysis, "dc") == 0 && mosfet == 'd') {
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
   // For AC Analysis of Deplation-Type MOSFET
   else if (strcmp(analysis, "ac") == 0 && mosfet == 'd') {
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
   // For DC Analysis of Enhancment-Type MOSFET
   else if (strcmp(analysis, "dc") == 0 && mosfet == 'e') {
      // Get inputs of drain-feedback config.
      _m_display_transistor("dc", &transistor);
      // For Drain-Feedback Configuration:
      if (strcmp(transistor, "df") == 0 || 
          strcmp(transistor, "fb") == 0) {
         // Get inputs of drain-feedback config.
         _m_drain_feedback_inputs_("dc", &Vdd, &Rg1, &Rd, &Idon, 
                                   &Vgson, &Vgsth, 1);
         // Calculate the results of drain-feedback config.
         m_drain_feedback("dc", Vdd, Rg1, Rd, Idon, Vgson, Vgsth, 1);
         // Display the results of drain-feedback config.
         printf("k: %f (A/V^2)\n", DCMOSFET.k);
         printf("Id: %f (A)\n", DCMOSFET.Id);
         printf("Vgs: %f (V)\n", DCMOSFET.Vgs);
         printf("Vds: %f (V)\n", DCMOSFET.Vds);
      }
      // For Voltage-Divider Configuration:
      else if (strcmp(transistor, "vd") == 0) {
         // Get inputs of voltage-divider config.
         _m_voltage_divider_inputs_("dc", &Vdd, &Rg1, &Rg2, &Rd, &Rs,
                                    &Idon, &Vgson, &Vgsth, 1);
         // Calculate the results of voltage-divider config.
         m_voltage_divider("dc", Vdd, Rg1, Rg2, Rd, Rs, Idon, Vgson,
                           Vgsth, 1);
         // Display the results of voltage-divider config.
         printf("k: %f (A/V^2)\n", DCMOSFET.k);
         printf("Id: %f (A)\n", DCMOSFET.Id);
         printf("Vgs: %f (V)\n", DCMOSFET.Vgs);
         printf("Vds: %f (V)\n", DCMOSFET.Vds);
      }
      else puts("Can not found that transistor !!!");
   }
   // For AC Analysis of Enhancment-Type MOSFET
   else if (strcmp(analysis, "ac") == 0 && mosfet == 'e') {
      // Get inputs of drain-feedback config.
      _m_display_transistor("ac", &transistor);
      // For Drain-Feedback Configuration:
      if (strcmp(transistor, "df") == 0 || 
          strcmp(transistor, "fb") == 0) {
         // Get inputs of drain-feedback config.
         _m_drain_feedback_inputs_("ac", &Vdd, &Rg1, &Rd, &Idon, 
                                   &Vgson, &Vgsth, &rd);
         // Calculate the results of drain-feedback config.
         m_drain_feedback("ac", Vdd, Rg1, Rd, Idon, Vgson, Vgsth, rd);
         // Display the results of drain-feedback config.
         printf("gm: %f (S)\n", ACMOSFET.gm);
         printf("Zi: %f (ohm)\n", ACMOSFET.Zi);
         printf("Zo: %f (ohm)\n", ACMOSFET.Zo);
         printf("Av: %f (V)\n", ACMOSFET.Av);
         printf("Phase: %s\n", ACMOSFET.phase);
      }
      // For Voltage-Divider Configuration:
      else if (strcmp(transistor, "vd") == 0) {
         // Get inputs of voltage-divider config.
         _m_voltage_divider_inputs_("ac", &Vdd, &Rg1, &Rg2, &Rd, &Rs,
                                    &Idon, &Vgson, &Vgsth, &rd);
         // Calculate the results of voltage-divider config.
         m_voltage_divider("ac", Vdd, Rg1, Rg2, Rd, Rs, Idon, Vgson,
                           Vgsth, rd);
         // Display the results of voltage-divider config.
         printf("gm: %f (S)\n", ACMOSFET.gm);
         printf("Zi: %f (ohm)\n", ACMOSFET.Zi);
         printf("Zo: %f (ohm)\n", ACMOSFET.Zo);
         printf("Av: %f (V)\n", ACMOSFET.Av);
         printf("Phase: %s\n", ACMOSFET.phase);
      }
      else puts("Can not found that transistor !!!");
   }
   else puts("Can not found that analysis or mosfet !!!");

}