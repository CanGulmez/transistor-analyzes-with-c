/* The DC And AC Analysis of All BJT Transistor Configurations

I've started this project for making easier the transistor and 
related operations. The important point is that all transistors are
'npn' types. But, can be derivated easily 'pnp' implementations. 
This package contains mostly used configuration and some special
connections. Another important point is that in AC analysis of
transistors, algorithms use 're' transistor model.

Resource: Electronic Devices and Circuit Theory by Robert L. 
Boylestad and Louis Nashelsky
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// General constants
#define Vbe 0.7

// Results of DC analysis
struct DCComponents {
   float Ib; // base current
   float Ic; // collector current 
   float Ie; // emitter current
   float Icsat; // collector saturation (max) current
   float Vce; // collector-emitter voltage
   float Vc; // collector voltage
   float Ve; // emitter voltage
   float Vb; // base voltage
   float Vbc; // base-collector voltage
};
// Results of AC analysis
struct ACComponents {
   float re; // re factor
   float Zi;  // input impedance
   float Zo; // output impedance
   float Av; // voltage gain
   char* phase; // phase relationship
};

/* For all DC configuration: */
struct DCComponents DCAnalysis;
/* For all AC configuration */
struct ACComponents ACAnalysis;

float _Rth_(float R1, float R2) {
   // Rth is necesarry for voltage divider config.
   return 1 / (1/R1 + 1/R2); 
}
float _Eth_(float Vcc, float R1, float R2) {
   // Eth is necesarry for voltage divider config.
   return Vcc * (R2 / (R1 + R2));
}

void _save_dc_results_(float Ib, float Ic, float Ie, float Icsat,
                       float Vce, float Vc, float Ve, float Vb, 
                       float Vbc) {
   // Save results into 'DCAnalysis' struct.
   DCAnalysis.Ib = Ib; // base current
   DCAnalysis.Ic = Ic; // collector current
   DCAnalysis.Ie = Ie; // emitter current
   DCAnalysis.Icsat = Icsat; // collector saturation (max) current
   DCAnalysis.Vce = Vce; // collector-emitter voltage
   DCAnalysis.Vc = Vc; // collector voltage
   DCAnalysis.Ve = Ve; // emitter voltage
   DCAnalysis.Vb = Vb; // base voltage
   DCAnalysis.Vbc = Vbc; // base-collector voltage
}

void _save_ac_results_(float re, float Zi, float Zo, float Av,       
                       char* phase) {
   // Save results into 'ACAnalysis' struct.
   ACAnalysis.re = re;
   ACAnalysis.Zi = Zi; // input impedance
   ACAnalysis.Zo = Zo; // output impedance
   ACAnalysis.Av = Av; // voltage gain
   ACAnalysis.phase = phase; // phase relationships
}

/* The DC and AC Analysis of Fixed-Bias Configuration */
void fixed_bias(char* analysis, float Vcc, float Rb, float Rc, 
                float beta, float ro) {
   // Check if the parameters are correct.
   assert (Rb > 0 && Rc > 0 && beta > 0 && ro > 0);
   // These parameters can be required for both analyzes.
   float Ib = (Vcc - Vbe) / Rb; // base current
   float Ie = (beta + 1) * Ib; // emitter current
   // Calculate the DC results.
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / Rc; // collector saturation current
   float Vce = Vcc - (Ic * Rc); // collector-emitter voltage
   float Vc = Vce; // collector voltage
   float Ve = 0; // emitter voltage
   float Vb = Vbe; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage
   // Calculate the AC results.
   float re = 0.026 / Ie; // re factor
   float Zi = 1 / (1/Rb + 1/(beta * re)); // input impedance
   float Zo = 1 / (1/Rc + 1/ro); // output impedance
   float Av = -1 * (1 / (1/Rc + 1/ro)) / re; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) // dc results
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   else // ac results
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
}

/* The DC and AC Analysis of Emitter-Bias Configuration */
void emitter_bias(char* analysis, float Vcc, float Rb, float Rc, 
                  float Re, float beta, float ro) {
   // Check if the parameters are correct.
   assert (Rb > 0 && Rc > 0 && Re > 0 && beta > 0 && ro > 0);
   // These parameters can be required for both analyzes.
   float Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   // Calculate the DC results.
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / (Rc + Re); // collector saturation current
   float Vce = Vcc - Ic * (Rc + Re); // collector-emitter voltage
   float Ve = Ie * Re; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage
   // Calculate the AC results.
   float re = 0.026 / Ie; // re factor
   float Zb1 = (beta + 1) + (Rc/ro);
   float Zb2 = 1 + (Rc + Re) / ro;
   float Zb = beta * re + (Zb1 / Zb2) * Re;
   float Zi = 1 / (1/Rb + 1/Zb); // input impedance
   float Zo1 = beta * (ro + re);
   float Zo2 = 1 + (beta * re) / Re;
   float Zo3 = ro + Zo1 / Zo2;
   float Zo = 1 / (1/Rc + 1/Zo3); // output impedance
   float Av1 = (-1 * (beta * Rc) / Zb) * (1 + (re/ro)) + (Rc/ro);
   float Av2 = 1 + (Rc / ro);
   float Av = Av1 / Av2; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) // dc results
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   else // ac results
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
}

/* The DC and AC Analysis of Voltage-Divider Configuration */
void voltage_divider(char* analysis, float Vcc, float Rb1, 
                     float Rb2, float Rc, float Re, float beta, 
                     float ro, char* bypass) {
   // Check if the parameters are correct.
   assert (Rb1 > 0 && Rb2 > 0 && Rc > 0 && Re > 0);
   assert (beta > 0 && ro > 0);
   // These parameters can be required for both analyzes.
   float rth = _Rth_(Rb1, Rb2); 
   float eth = _Eth_(Vcc, Rb1, Rb2);
   float Ib = (eth - Vbe) / (rth +(beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor
   // Calculate the DC results.
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / (Rc + Re); // collector saturation current
   float Vce = Vcc - Ic * (Rc + Re); // collector-emitter current
   float Ve = Ie * Re; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage
   // Calculate the AC results.
   float Zb1, Zb2, Zb, Zi, Zo1, Zo2, Zo3, Zo, Av1, Av2, Av;
   // Specialize the calculations according to 'bypass' parameter.
   if (strcmp(bypass, "bypassed") == 0) {
      Zi = 1 / (1 /rth + 1 /(beta * re)); // input impedance
      Zo = 1 / (1/Rc + 1/ro); // output impedance
      Av = -1 * (1 / (1/Rc + 1/ro)) / re; // voltage gain
   } else {
      Zb1 = (beta + 1) + (Rc/ro);
      Zb2 = 1 + (Rc + Re) / ro;
      Zb = beta * re + (Zb1 / Zb2) * Re;
      Zi = 1 / (1/rth + 1/Zb); // input impedance
      Zo1 = beta * (ro + re);
      Zo2 = 1 + (beta * re) / Re;
      Zo3 = ro + Zo1 / Zo2;
      Zo = 1 / (1/Rc + 1/Zo3); // output impedance
      Av1 = (-1 * (beta * Rc) / Zb) * (1 + (re/ro)) + (Rc/ro);
      Av2 = 1 + (Rc / ro);
      Av = Av1 / Av2; // voltage gain
   }
   // Save the results.
   if (strcmp(analysis, "dc") == 0) // dc results
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   else // ac results
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
}

void collector_feedback(char* analysis, float Vcc, float Rf, 
                        float Rc, float Re, float beta, float ro) {
   // Check if the parameters are correct.
   assert (Rf > 0 && Rc > 0 && Re > 0 && beta > 0 && ro > 0);
   // These parameters can be required for both analyzes.
   float Ib;
   if (strcmp(analysis, "ac")) // if 'analysis' is dc.
   Ib = (Vcc - Vbe) / (Rf + beta * (Rc+Re)); // base current
   else // if 'analysis' is ac.
   Ib = (Vcc - Vbe) / (Rf + beta * Rc); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   // Calculate the DC results.
   float Ic = beta * Ib; // collector current
   float Icsat = Vcc / (Rc + Re); // collector saturation current
   float Vce = Vcc - Ic * (Rc + Re); // collector-emitter voltage
   float Ve = Ie * Re; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage
   // Calculate the AC results.
   float re = 0.026 / Ie; // re factor
   float Zi1 = 1 + (_Rth_(Rc, ro) / Rf);
   float Zi2 = (1 / (beta * re)) + (1 / Rf);
   float Zi3 = _Rth_(Rc, ro) / (beta * re * Rf);
   float Zi4 = _Rth_(Rc, ro) / (Rf * re);
   float Zi = Zi1 / (Zi2 + Zi3 + Zi4); // input impedance
   float Zo = 1 / (1/ro + 1/Rc + 1/Rf); // output impedance
   float Av1 = Rf / (_Rth_(Rc, ro) + Rf);
   float Av2 = _Rth_(Rc, ro) / re;
   float Av = -1 * Av1 * Av2; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) // dc results
   _save_dc_results_(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
   else // ac results
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
}

/* The AC Analysis of Collector-DC-Feedback Configuration */
void collector_dc_feedback(char* analysis, float Vcc, float Rf1,
                           float Rf2, float Rc, float beta, 
                           float ro) {
   // Check if the parameters are correct.
   assert (Rf1 > 0 && Rf2 > 0 && Rc > 0 && beta > 0 && ro > 0); 
   // Calculate AC the results.
   float Ib = (Vcc - Vbe) / (Rf1+Rf2 + (beta * Rc)); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   float re = 0.026 / Ie; // re factor
   float Zi = 1 / (1/Rf1 + 1/(beta * re)); // input impedance
   float Zo = 1 / (1/Rc + 1/Rf2 + 1/ro); // output impedance
   float Av = -1 * Zo / re; // voltage gain
   if (strcmp(analysis, "dc") == 0 ) { // dc results
   puts("Transistor do not support dc analysis !!!"); 
   exit(EXIT_FAILURE); }
   else // ac results
   _save_ac_results_(re, Zi, Zo, Av, "Out of phase");
}

/* The DC and AC Analysis of Emitter-Follower Configuration */
void emitter_follower(char* analysis, float Vcc, float Vee, 
                      float Rb, float Re, float beta, float ro) {
   // Check if the parameters are correct.
   assert (Rb > 0 && Re > 0 && beta > 0 && ro > 0);
   // These parameters can be required for both analyzes.
   float Ib; 
   if (strcmp(analysis, "ac")) // dc analysis
   Ib = (Vee - Vbe) / (Rb + (beta + 1) * Re); // base current
   else // ac analysis
   Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re); // base current
   float Ie = (beta + 1) * Ib; // emitter current
   // Calculate the DC results.
   float Ic = beta * Ib; // collector current
   float Vce = Vee - (Ie * Re); // collector-enitter voltage
   float Ve = (Ie * Re) + Vee; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage
   // Calculate the AC results.
   float re = 0.026 / Ie; // re factor
   float Zb1 = (beta + 1) * Re;
   float Zb2 = 1 + (Re / ro);
   float Zb = (beta * re) + (Zb1 / Zb2);
   float Zi = 1 / (1 /Rb + 1 /Zb); // input impedance
   float Zo1 = (beta * re) / (beta + 1);
   float Zo = 1 / (1 /ro + 1 /Re + 1 /Zo1); // output impedance
   float Av1 = (beta + 1) * Re / Zb;
   float Av = Av1 / (1 + (Re/ro)); // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) // dc results
   _save_dc_results_(Ib, Ic, Ie, -1.0, Vce, Vc, Ve, Vb, Vbc);
   else // ac results
   _save_ac_results_(re, Zi, Zo, Av, "In phase");
}

/* The DC and AC Analysis of Common-Base COnfiguration */
void common_base(char* analysis, float Vcc, float Vee, float Rc,
                 float Re, float beta, float alpha) {
   // Check if the parameters are correct.
   assert (Rc > 0 && Re > 0 && beta > 0 && alpha > 0);
   // These parameters can be required for both analyzes.
   float Ie = (Vee - Vbe) / Re; // emitter current
   // Calculate the DC results.
   float Ib = Ie / (beta + 1); // base current
   float Ic = Ib * beta; // collector current
   float Vce = Vee + Vcc - Ie * (Rc + Re); // collector-emitter v.
   float Vcb = Vcc - Ic * Rc; // collector-base voltage
   float Vbc = -1 * Vcb; // base-collector voltage
   // Calculate the AC results.
   float re = 0.026 / Ie; // re factor
   float Zi = 1 / (1/Re + 1/re); // input impedance
   float Zo = Rc; // output impedance
   float Av = alpha * Rc / re; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) // dc results
   _save_dc_results_(Ib, Ic, Ie, -1.0, Vce, -1.0, -1.0, -1.0, Vbc);
   else // ac results
   _save_ac_results_(re, Zi, Zo, Av, "In phase");
}

/* The DC Analysis of Miscellaneous-Bias COnfiguration */
void miscellaneous_bias(char* analysis, float Vcc, float Rb, 
                        float Rc, float beta) {
   // Check if the parameters are correct.
   assert (Rb > 0 && Rc > 0 && beta > 0);
   // Calculate DC the results.
   float Ib = (Vcc - Vbe) / (Rb + beta * Rc); // base current
   float Ic = beta * Ib; // collector current
   float Ie = (beta + 1) * Ib; // emitter current
   float Vce = Vcc - (Ie * Rc); // collector-emitter voltage
   float Ve = 0; // emitter voltage
   float Vc = Vce + Ve; // collector voltage
   float Vb = Vbe + Ve; // base voltage
   float Vbc = Vb - Vc; // base-collector voltage
   // Save the results into struct.
   if (strcmp(analysis, "dc") == 0) // dc results
   _save_dc_results_(Ib, Ic, Ie, -1.0, Vce, Vc, Ve, Vb, Vbc);
   else { // ac results
   puts("Transistor do not support ac analysis !!!"); 
   exit(EXIT_FAILURE); }
}

void _display_transistors_(char* analysis, char* transistor) {
   // Display the all transistors.
   puts("TRANSISTOR:");
   puts("--> 'fb' for fixed-bias config.");
   puts("--> 'eb' for emitter-bias config.");
   puts("--> 'vd' for voltage-divider config.");
   puts("--> 'cf' for collector-feedback config.");
   if (strcmp(analysis, "ac") == 0)
      puts("--> 'cdf' for collector-dc-feedback config.");
   puts("--> 'ef' for emitter-follower config.");
   puts("--> 'cb' for common-base config.");
   if (strcmp(analysis, "dc") == 0) 
      puts("--> 'mb' for miscellaneous-bias config.");
   printf("Transistor type: "); scanf("%s", &(*transistor));
   puts("-------------------------------------------");
}

void _fixed_bias_inputs_(char* analysis, float* Vcc, float* Rb, 
                         float* Rc, float* beta, float* ro) {
   // Get inputs of fixed-bias configuration.
   puts("PARAMETERS: ");
   // DC parameters:
   printf("Vcc (volt): "); scanf("%f", &(*Vcc));
   printf("Rb (ohm): "); scanf("%f", &(*Rb));
   printf("Rc (ohm): "); scanf("%f", &(*Rc));
   printf("Beta (unitless): "); scanf("%f", &(*beta));
   // AC parameters:
   if (strcmp(analysis, "ac") == 0) {
      printf("ro (ohm): "); scanf("%f", &(*ro)); 
   }
   puts("Calculating results ...");
   puts("-------------------------------------------");
}

void _emitter_bias_inputs_(char* analysis, float* Vcc, float* Rb,
                           float* Rc, float* Re, float* beta, 
                           float* ro) {
   // Get inputs of emitter-bias configuration.
   puts("PARAMETERS: ");
   // DC parameters:
   printf("Vcc (volt): "); scanf("%f", &(*Vcc));
   printf("Rb (ohm): "); scanf("%f", &(*Rb));
   printf("Rc (ohm): "); scanf("%f", &(*Rc));
   printf("Re (ohm): "); scanf("%f", &(*Re));
   printf("Beta (unitless): "); scanf("%f", &(*beta));
   // AC parameters:
   if (strcmp(analysis, "ac") == 0) {
      printf("ro (ohm): "); scanf("%f", &(*ro)); 
   }
   puts("Calculating results ...");
   puts("-------------------------------------------");
}

void _voltage_divider_inputs_(char* analysis, float* Vcc, 
                              float* Rb1, float* Rb2, float* Rc,
                              float* Re, float* beta, float* ro, 
                              char* bypass) {
   // Specialize the results according to 'bypass' argument.
   if (strcmp(analysis, "dc")) {
      puts("BYPASS:");
      printf("--> 'bypassed' for the short-circuit of");
      printf(" \n    emitter terminal\n");
      printf("--> 'unbypassed' for the not short cir-\n");
      printf("    cuit of emitter terminal\n");
      printf("Bypass of transistor: "); scanf("%s", &(*bypass));
      puts("-------------------------------------------");
   }
   // Get inputs of emitter-bias configuration.
   puts("PARAMETERS: ");
   // DC parameters:
   printf("Vcc (volt): "); scanf("%f", &(*Vcc));
   printf("Upper Rb (ohm): "); scanf("%f", &(*Rb1));
   printf("Lower Rb (ohm): "); scanf("%f", &(*Rb2));
   printf("Rc (ohm): "); scanf("%f", &(*Rc));
   printf("Re (ohm): "); scanf("%f", &(*Re));
   printf("Beta (unitless): "); scanf("%f", &(*beta));
   // AC parameters:
   if (strcmp(analysis, "ac") == 0) {
      printf("ro (ohm): "); scanf("%f", &(*ro)); 
   }
   puts("Calculating results ...");
   puts("-------------------------------------------");
}

void _collector_feedback_inputs_(char* analysis, float* Vcc, 
                                 float* Rf, float* Rc, float* Re, 
                                 float* beta, float* ro) {
   // Get inputs of collector-feedback configuration.
   puts("PARAMETERS:");
   // DC parameters:
   printf("Vcc (volt): "); scanf("%f", &(*Vcc));
   printf("Rf (ohm): "); scanf("%f", &(*Rf));
   printf("Rc (ohm): "); scanf("%f", &(*Rc));
   // AC parameters:
   if (strcmp(analysis, "ac")) {
   printf("Re (ohm): "); scanf("%f", &(*Re));
   }
   printf("Beta (unitless): "); scanf("%f", &(*beta));
   // AC parameters:
   if (strcmp(analysis, "ac") == 0) {
      printf("ro (ohm): "); scanf("%f", &(*ro)); 
   }
   puts("Calculating results...");
   puts("-------------------------------------------");
}

void _collector_dc_feedback_inputs_(char* analysis, float* Vcc, 
                                    float* Rf1, float* Rf2, 
                                    float* Rc, float* beta, 
                                    float* ro) {
   // Get inputs of collector-feedback configuration.
   puts("PARAMETERS:");
   // DC parameters:
   printf("Vcc (volt): "); scanf("%f", &(*Vcc));
   printf("Left Rf (ohm): "); scanf("%f", &(*Rf1));
   printf("Right Rf (ohm): "); scanf("%f", &(*Rf2));
   printf("Rc (ohm): "); scanf("%f", &(*Rc));
   printf("Beta (unitless): "); scanf("%f", &(*beta));
   // AC parameters:
   if (strcmp(analysis, "ac") == 0) {
      printf("ro (ohm): "); scanf("%f", &(*ro)); 
   }
   puts("Calculating results...");
   puts("-------------------------------------------");
}

void _emitter_follower_inputs_(char* analysis, float* Vcc, 
                               float* Vee, float* Rb, float* Re, 
                               float* beta, float* ro) {
   // Get inputs of emitter-follower configuration.
   puts("PARAMETERS:");
   if (strcmp(analysis, "ac")) { // dc analysis
      printf("Vee (volt): "); scanf("%f", &(*Vee)); 
   }
   else { // ac analysis
      printf("Vcc (volt): "); scanf("%f", &(*Vcc)); 
   }
   // DC parameters:
   printf("Rb (ohm): "); scanf("%f", &(*Rb));
   printf("Re (ohm): "); scanf("%f", &(*Re));
   printf("Beta (unitless): "); scanf("%f", &(*beta));
   // AC parameters:
   if (strcmp(analysis, "dc")) {
      printf("ro (ohm): "); scanf("%f", &(*ro)); 
   }
   puts("Calculating results...");
   puts("-------------------------------------------");
}

void _common_base_inputs_(char* analysis, float* Vcc, float* Vee, 
                          float* Rc, float* Re, float* beta, 
                          float* alpha) {
   // Get inputs of emitter-follower configuration.
   puts("PARAMETERS: ");
   printf("Vcc (volt): "); scanf("%f", &(*Vcc));
   printf("Vee (volt): "); scanf("%f", &(*Vee));
   printf("Rc (ohm): "); scanf("%f", &(*Rc));
   printf("Re (ohm): "); scanf("%f", &(*Re));
   // DC parameters:
   if (strcmp(analysis, "ac")) {
      printf("Beta (unitless): "); scanf("%f", &(*beta));
   }
   else {
      printf("Alpha (unitless): "); scanf("%f", &(*alpha));
   }
   puts("Calculating results ...");
   puts("-------------------------------------------");
}

void _miscellaneous_bias_inputs_(char* analysis, float* Vcc, 
                                 float* Rb, float* Rc, float* beta) {
   // Get inputs of miscellaneous-bias configuration.
   puts("PARAMETERS: ");
   printf("Vcc (volt): "); scanf("%f", &(*Vcc));
   printf("Rb (ohm): "); scanf("%f", &(*Rb));
   printf("Rc (ohm): "); scanf("%f", &(*Rc));
   printf("Beta (unitless): "); scanf("%f", &(*beta));
   puts("Calculating results ...");
   puts("-------------------------------------------");
}

void _display_dc_results_(char* transistor) {
   // Display the DC results.
   puts("RESULTS: ");
   printf("Ib: %e A\n", DCAnalysis.Ib);
   printf("Ic: %e A\n", DCAnalysis.Ic);
   printf("Ie: %e A\n", DCAnalysis.Ie);
   if (strcmp(transistor, "ef") && strcmp(transistor, "cb") &&
       strcmp(transistor, "mb"))
   printf("Ic sat.: %e A\n", DCAnalysis.Icsat);
   printf("Vce: %f V\n", DCAnalysis.Vce);
   if (strcmp(transistor, "cb")) {
   printf("Vc: %f V\n", DCAnalysis.Vc);
   printf("Ve: %f V\n", DCAnalysis.Ve);
   printf("Vb: %f V\n", DCAnalysis.Vb);
   }
   printf("Vbc: %f V\n", DCAnalysis.Vbc);
   printf("Vbe: %f V\n", Vbe);
   puts("-------------------------------------------");
}

void _display_ac_results_(void) {
   // Display the AC results
   puts("RESULTS: ");
   printf("re: %f ohm\n", ACAnalysis.re);
   printf("Zi: %f ohm\n", ACAnalysis.Zi);
   printf("Zo: %f ohm\n", ACAnalysis.Zo);
   printf("Av: %f\n", ACAnalysis.Av);
   printf("phase: %s\n", ACAnalysis.phase);
   puts("-------------------------------------------");
}

/* Main method that will display the all implemnetations */
int main(void) {
   // 'analysis' argument represents the type of analysis.
   char analysis[2];
   // 'transistor' argument represents type of transistor.
   char transistor[2];
   // These arguments represent the values of a transistor. 
   float Vcc, Rb1, Rb2, Rc, beta, Re, Rf1, Vee, ro, Rf2, alpha;
   // Determine the AC analysis of voltage-divider config status.
   char bypass[10];

   puts("-------------------------------------------");
   puts("  WELLCOME TO ANALYSIS OF BJT TRANSISTORS  ");
   puts("-------------------------------------------");
   puts("ANALYSIS:");
   puts("--> 'dc' for direct current");
   puts("--> 'ac' for alternative current");
   printf("Analysis mode: "); scanf("%s", &analysis);
   puts("-------------------------------------------");

   // For DC Analysis:
   if (strcmp(analysis, "dc") == 0) {
      // Display and get the transistors and types. 
      _display_transistors_("dc", &transistor);
      // For Fixed-Bias Configuration:
      if (strcmp(transistor, "fb") == 0) {
         // Get inputs of fixed-bias config.
         _fixed_bias_inputs_("dc", &Vcc, &Rb1, &Rc, &beta, 1);
         // Calculate the results of fixed-bias config.
         fixed_bias("dc", Vcc, Rb1, Rc, beta, 1);
         // Display the results of fixed-bias config.
         _display_dc_results_("dc");
      } 
      // For Emitter-Bias Configuration:
      else if (strcmp(transistor, "eb") == 0) {
         // Get inputs of emitter-bias config.
         _emitter_bias_inputs_("dc", &Vcc, &Rb1, &Rc, &Re, &beta, 1);
         // Calculate the results of emitter-bias config.
         emitter_bias("dc", Vcc, Rb1, Rc, Re, beta, 1);
         // Diplay the results of emitter-bias config.
         _display_dc_results_("em");
      }
      // For Voltage-Divider Configuration:
      else if (strcmp(transistor, "vd") == 0) {
         // Get inputs of voltage-divider config.
         _voltage_divider_inputs_("dc", &Vcc, &Rb1, &Rb2, &Rc, &Re,
                                  &beta, 1, "Undefined");
         // Calculate the results of voltage-divider config.
         voltage_divider("dc", Vcc, Rb1, Rb2, Rc, Re, beta, 
                         1, "Undefined");
         // Display the results of voltage-divider config.
         _display_dc_results_("dv");
      }
      // For Collector-Feedback Configuration:
      else if (strcmp(transistor, "cf") == 0) {
         // Get inputs of collector-feedback config.
         _collector_feedback_inputs_("dc", &Vcc, &Rf1, &Rc, &Re, 
                                     &beta, 1);
         // Calculate the results of collector-feedback config.
         collector_feedback("dc", Vcc, Rf1, Rc, Re, beta, 1);
         // Display the results of collector-feedback config.
         _display_dc_results_("cf");
      }
      // For Emitter-Follower Configuration:
      else if (strcmp(transistor, "ef") == 0) {
         // Get inputs of emitter-follower config.
         _emitter_follower_inputs_("dc", 1, &Vee, &Rb1, &Re, 
                                   &beta, 1);
         // Calculate the results of emitter-follower config.
         emitter_follower("dc", 1, Vee, Rb1, Re, beta, 1);
         // Display the results of emitter-follower config.
         _display_dc_results_("ef");
      }
      // For Common-Base Configuration:
      else if (strcmp(transistor, "cb") == 0) {
         // Get inputs of common-base config.
         _common_base_inputs_("dc", &Vcc, &Vee, &Rc, &Re, &beta, 1);
         // Calculate the results of common-base config.
         common_base("dc", Vcc, Vee, Rc, Re, beta, 1);
         // Display the results of common-base config.
         _display_dc_results_("cb");
      }
      // For Miscellaneous-Bias Configuration:
      else if (strcmp(transistor, "mb") == 0) {
         // Get inputs of miscellaneous-bias config.
         _miscellaneous_bias_inputs_("dc", &Vcc, &Rb1, &Rc, &beta);
         // Calculate the results of miscellaneous-bias config.
         miscellaneous_bias("dc", Vcc, Rb1, Rc, beta);
         // Display the results of miscellaneous-bias config.
         _display_dc_results_("mb");
      }
      else puts("Can not found that transistor !!!");
   }
   // For AC analysis:
   else if (strcmp(analysis, "ac") == 0) {
      // Display and get the transistors and types. 
      _display_transistors_("ac", &transistor);
      // For Fixed-Bias Configuration:
      if (strcmp(transistor, "fb") == 0) {
         // Get inputs of fixed-bias config.
         _fixed_bias_inputs_("ac", &Vcc, &Rb1, &Rc, &beta, &ro);
         // Calculate the results of fixed-bias config.
         fixed_bias("ac", Vcc, Rb1, Rc, beta, ro);
         // Display the results of fixed-bias config.
         _display_ac_results_();
      } 
      // For Emitter-Bias Configuration:
      else if (strcmp(transistor, "eb") == 0) {
         // Get inputs of emitter-bias config.
         _emitter_bias_inputs_("ac", &Vcc, &Rb1, &Rc, &Re,
                               &beta, &ro);
         // Calculate the results of emitter-bias config.
         emitter_bias("ac", Vcc, Rb1, Rc, Re, beta, ro);
         // Diplay the results of emitter-bias config.
         _display_ac_results_();
      }
      // For Voltage-Divider Configuration:
      else if (strcmp(transistor, "vd") == 0) {
         // Get inputs of voltage-divider config.
         _voltage_divider_inputs_("ac", &Vcc, &Rb1, &Rb2, &Rc, &Re, 
                                  &beta, &ro, &bypass);
         // Calculate the results of voltage-divider config.
         if (strcmp(bypass, "bypassed") == 0) 
         voltage_divider("ac", Vcc, Rb1, Rb2, Rc, Re, beta, ro, 
                         "bypassed");
         else if (strcmp(bypass, "unbypassed") == 0)
         voltage_divider("ac", Vcc, Rb1, Rb2, Rc, Re, beta, ro, 
                         "unbypassed");
         else puts("Can not found that bypass !!!");
         // Display the results of voltage-divider config.
         _display_ac_results_();
      }
      // For Collector-Feedback Configuration:
      else if (strcmp(transistor, "cf") == 0) {
         // Get inputs of collector-feedback config.
         _collector_feedback_inputs_("ac", &Vcc, &Rf1, &Rc, 1, 
                                     &beta, &ro);
         // Calculate the results of collector-feedback config.
         collector_feedback("ac", Vcc, Rf1, Rc, 1, beta, ro);
         // Display the results of collector-feedback config.
         _display_ac_results_();
      }
      // For Collector-DC-Feedback Configuration:
      else if (strcmp(transistor, "cdf") == 0) {
         // Get inputs of collector-dc-feedback config.
         _collector_dc_feedback_inputs_("ac", &Vcc, &Rf1, &Rf2, 
                                        &Rc, &beta, &ro);
         // Calculate the results of collector-dc-feedback config.
         collector_dc_feedback("ac", Vcc, Rf1, Rf2, Rc, beta, ro);
         // Display the results of collector-dc-feedback config.
         _display_ac_results_();
      }
      // For Emitter-Follower Configuration:
      else if (strcmp(transistor, "ef") == 0) {
         // Get inputs of emitter-follower config.
         _emitter_follower_inputs_("ac", &Vcc, 1, &Rb1, &Re, 
                                   &beta, &ro);
         // Calculate the results of emitter-follower config.
         emitter_follower("ac", Vcc, 1, Rb1, Re, beta, ro);
         // Display the results of emitter-follower config.
         _display_ac_results_();
      }
      // For Common-Base Configuration:
      else if (strcmp(transistor, "cb") == 0) {
         // Get inputs of common-base config.
         _common_base_inputs_("ac", &Vcc, &Vee, &Rc, &Re, 1, &alpha);
         // Calculate the results of common-base config.
         common_base("ac", Vcc, Vee, Rc, Re, 1, alpha);
         // Display the results of common-base config.
         _display_ac_results_();
      }
      else puts("Can not found that transistor !!!");
   }
   else puts("Can not found that analysis !!!");

   return 1;
}