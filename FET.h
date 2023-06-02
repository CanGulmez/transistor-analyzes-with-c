/* Basics FET Operations and Calculations */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

// Results of DC Analysis
struct DCComponents {
   float Id; // drain current
   float Vgs; // gate-source voltage
   float Vds; // drain-source voltage
   float Vs; // source voltage
   float Vd; // drain voltage
   float Vg; // gate voltage
};
// Results of AC Analysis
struct ACComponents {
   float gm; // transconductance factor
   float Zi; // input impedance
   float Zo; // output impedance
   float Av; // voltage gain
   char* phase; // phase relationship
};

// For all DC configuration:
struct DCComponents DCAnalysis;
// For all AC configuration:
struct ACComponents ACAnalysis;

void _save_dc_results_(float Id, float Vds, float Vgs, float Vs, 
                       float Vd, float Vg) {
   // Save the results into 'DCAnalysis' struct.
   DCAnalysis.Id = Id; // drain current
   DCAnalysis.Vds = Vds; // drain-source voltage
   DCAnalysis.Vgs = Vgs; // gate-source voltage
   DCAnalysis.Vs = Vs; // source voltage
   DCAnalysis.Vd = Vd; // drain voltage
   DCAnalysis.Vg = Vg; // gate voltage
}

void _save_ac_results_(float gm, float Zi, float Zo, float Av, 
                       char* phase) {
   // Save the results into 'ACAnalysis' strcut.
   ACAnalysis.gm = gm; // transconductance factor
   ACAnalysis.Zi = Zi; // input impedance
   ACAnalysis.Zo = Zo; // output impedance
   ACAnalysis.Av = Av; // voltage gain
   ACAnalysis.phase = phase; // phase relationship
}

float _select_right_Id_(float a, float b, float c) {
   // Find dicriminant and calculate two different roots.
   float dicriminant = (b * b) - (4 * a * c);
   float root1 = (-1.0 * b + sqrt(dicriminant)) / (2 * a); 
   float root2 = (-1.0 * b - sqrt(dicriminant)) / (2 * a);
   // Specially, in some configuration, can be found two root
   // and requires slecting one.
   if (root1 >= 0 && root2 < 0) return root1;
   if (root2 >= 0 && root1 < 0) return root2;
   if (root1 >= 0 && root2 >= 0) {
      if (root1 >= root2) return root2;
      else return root1;
   } 
   if (root1 < 0 && root2 < 0) {
      if (abs(root1) >= abs(root2)) return abs(root2);
      else return abs(root1);
   }
}

float _find_gm_factor_(float Idss, float Vp, float Vgs) {
   // Find transconductance factor (gm).
   return (2.0 * Idss / abs(Vp)) * (1.0 - Vgs / Vp); 
}

float _parallel_(float R1, float R2) {
   // Find resultant resistance for two paallel resistances.
   return 1.0 / (1.0/R1 + 1.0/R2);
}

/* The DC and AC Analysis of Fixed-Bias Configuration */
void fixed_bias(char* analysis, float Vdd, float Vgg, float Rg, 
                float Rd, float Idss, float Vp, float rd) {
   // Check if the parameters are correct.
   assert (Rg > 0 && Rd > 0 && rd > 0);
   // These results are required for both analysis type.
   float Vgs = -1 * Vgg; // gate-source voltage
   // Calculate DC analysis:
   float Id1 = (1.0 - (Vgs / Vp)) * (1.0 - (Vgs / Vp));
   float Id = Idss * Id1;  // drain current
   float Vds = Vdd - Id * Rd; // drain-source voltage
   float Vd = Vds; // drain voltage
   float Vg = Vgs; // gate voltage
   float Vs = 0; // source voltage
   // AC analysis:
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm
   float Zi = Rg; // input impedance
   float Zo = _parallel_(Rd, rd); // output impedance
   float Av = -1.0 * gm * Zo; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) 
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   else _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
}


/* The DC and AC Analysis of Self-Bias Configuration */
void self_bias(char* analysis, float Vdd, float Rg, float Rd, float Rs,
               float Idss, float Vp, float rd) {
   // Check if the parameters are correct.
   assert (Rg > 0 && Rd > 0 && rd > 0 && Rs > 0);
   // These results are required for both analysis type.
   float a = Rs * Rs * Idss / Vp / Vp;
   float b = Idss * 2.0 * Rs / Vp - 1;
   float c = Idss;
   // Can be used dicriminant for finding right 'Id'.
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = -1 * Id * Rs; // gate-source voltage
   // DC analysis:
   float Vds = Vdd - Id * (Rs + Rd); // drain-source voltage
   float Vs = Id * Rs; // source voltage
   float Vg = 0; // gate voltage
   float Vd = Vds + Vs; // drain voltage
   // AC analysis:
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm factor
   float Zi = Rg; // input impedance
   float Zo1 = 1 + gm * Rs + Rs / rd;
   float Zo2 = 1 + gm * Rs + Rs / rd + Rd / rd;
   float Zo = Zo1 * Rd / Zo2; // output impedance
   float Av1 = gm * Rd;
   float Av2 = 1.0 + gm * Rs + (Rd + Rs) / rd;
   float Av = -1.0 * Av1 / Av2; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) 
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   else _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
}

/* The DC and AC Analysis of Voltage-Divider Configuration */
void voltage_divider(char* analysis, float Vdd, float Rg1, float Rg2, 
                     float Rd, float Rs, float Idss, float Vp, 
                     float rd){
   // Check if the parameters are correct.
   assert (Rg1 > 0 && Rg2 > 0 && Rd > 0 && Rs > 0 && rd > 0);
   // These results are required for both analysis type.
   float Vg = (Rg2 * Vdd) / (Rg1 + Rg2);
   // For quadritic equations, find discriminant.
   float a = Rs * Rs * Idss / Vp / Vp;
   float b1 = (2.0 * Rs * Idss / Vp);
   float b2 = (2.0 * Vg * Rs * Idss / Vp / Vp);
   float b = b1 - b2 - 1;
   float c = Idss * (1.0 - (2.0 * Vg / Vp) + (Vg * Vg / Vp / Vp));
   // Can be used dicriminant for finding right 'Id'.
   float Id = _select_right_Id_(a, b, c); // drain current
   float Vgs = Vg - Id * Rs;  // gate-source voltage
   // DC analysis:
   float Vds = Vdd - Id * (Rs + Rd); // drain-source voltage
   float Vs = Id * Rs; // source voltage
   float Vd = Vdd - Id * Rd; // drain voltage
   // AC analysis:
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm factor
   float Zi = _parallel_(Rg1, Rg2); // input impedance
   float Zo = _parallel_(Rd, rd); // output impedance
   float Av = -1 * gm * Zo; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) 
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   else _save_ac_results_(gm, Zi, Zo, Av, "Out of phase");
}

/* The DC and AC Analysis of Common-Gate Configuration */
void common_gate(char* analysis, float Vdd, float Vss, float Rd, 
                 float Rs, float Idss, float Vp, float rd) {
   // Check if the parameters are correct.
   assert (Rd > 0 && Rs > 0 && rd > 0);
   // For quadritic equations, find discriminant.
   float a = (Rs * Rs) * Idss / (Vp * Vp);
   float b1 = 2.0 * Rs * Idss / Vp;
   float b2 = -2.0 * Vss * Rs * Idss / Vp / Vp;
   float b = b1 + b2 - 1.0;
   float c1 = 2.0 * Vss / Vp;
   float c2 = Vss * Vss / Vp / Vp;
   float c = (1.0 - c1 + c2) * Idss;
   // Can be used dicriminant for finding right 'Id'.
   float Id = _select_right_Id_(a, b, c); // drain current
   // These results are required for both analysis type.
   float Vgs = Vss - Id * Rs; // gate-source voltage
   // DC analysis:
   float Vds = Vdd + Vss - Id * (Rs + Rd); // drain-source voltage
   float Vs = -Vss + Id * Rs; // source voltage
   float Vd = Vdd - Id * Rd; // drain voltage
   float Vg = 0; // gate voltage
   // AC analysis:
   float gm = _find_gm_factor_(Idss, Vp, Vgs); // gm factor
   float Zi1 = (rd + Rd) / (1 + gm * rd);
   float Zi = _parallel_(Rs, Zi1); // input impedance
   float Zo = _parallel_(Rd, rd); // output impedance
   float Av1 = gm * Rd + Rd / rd;
   float Av2 = 1 + Rd / rd;
   float Av = Av1 / Av2; // voltage gain
   // Save the results.
   if (strcmp(analysis, "dc") == 0) 
   _save_dc_results_(Id, Vds, Vgs, Vs, Vd, Vg);
   else _save_ac_results_(gm, Zi, Zo, Av, "In phase");
}

/* The AC Analysis of Source-Follower Configuration */
void source_follower(char* analysis, float Vdd, float Vgs, float Rg, 
                     float Rs, float Idss, float Vp, float rd) {
   // Check if the parameters are correct.
   assert (Rg > 0 && Rs > 0 && rd > 0);
   // Calculate the results.
   float gm = _find_gm_factor_(Idss, Vp, Vgs);
   float Zi = Rg;
   float Zo = _parallel_(rd, _parallel_(Rs, 1/gm));
   float Av1 = gm * _parallel_(rd, Rs);
   float Av2 = 1.0 + Av1;
   float Av = Av1 / Av2;
   // Save the results.
   if (strcmp(analysis, "ac")) { // dc results
      puts("Transistor do not support dc analysis !!!");
      exit(EXIT_FAILURE); }
   else // ac results
   _save_ac_results_(gm, Zi, Zo, Av, "In phase");
}


void _display_transistors_(char* analysis, char* transistor) {
   // Display the all transistors.
   puts("TRANSISTOR:");
   puts("--> 'fb' for fixed-bias config.");
   puts("--> 'sb' for self-bias config.");
   puts("--> 'vd' for voltage-divider config.");
   puts("--> 'cg' for common-gate config.");
   if (strcmp(analysis, "dc"))
      puts("--> 'sf' for source-follower config.");
   printf("Transistor type: "); scanf("%s", &(*transistor));
   puts("--------------------------------------------");
}

void _fixed_bias_inputs_(char* analysis, float* Vdd, float* Vgg, 
                        float* Rg, float* Rd, float* Idss, 
                        float* Vp, float* rd) {
   // Get inputs of fixed-bias configuration.
   puts("PARAMETERS: ");
   printf("Vdd (V): "); scanf("%f", &(*Vdd));
   printf("Vgg (V): "); scanf("%f", &(*Vgg));
   if (strcmp(analysis, "dc")) {
      printf("Rg (ohm): "); scanf("%f", &(*Rg));
   }
   printf("Rd (ohm): "); scanf("%f", &(*Rd));
   printf("Idss (A): "); scanf("%f", &(*Idss));
   printf("Vp (V): "); scanf("%f", &(*Vp));
   if (strcmp(analysis, "dc")) {
      printf("rd (ohm): "); scanf("%f", &(*rd));
   }
   puts("Calculating results ...");
   puts("--------------------------------------------");
}

void _self_bias_inputs_(char* analysis, float* Vdd, float* Rg, 
                        float* Rd, float* Rs, float* Idss, 
                        float* Vp, float* rd) {
   // Get inputs of self-bias configuration.
   puts("PARAMETERS: ");
   printf("Vdd (V): "); scanf("%f", &(*Vdd));
   if (strcmp(analysis, "dc")) {
      printf("Rg (ohm): "); scanf("%f", &(*Rg));
   }
   printf("Rd (ohm): "); scanf("%f", &(*Rd));
   printf("Rs (ohm): "); scanf("%f", &(*Rs));
   printf("Idss (A): "); scanf("%f", &(*Idss));
   printf("Vp (V): "); scanf("%f", &(*Vp));
   if (strcmp(analysis, "dc")) {
      printf("rd (ohm): "); scanf("%f", &(*rd));
   }
   puts("Calculating results ...");
   puts("--------------------------------------------");
}

void _voltage_divider_inputs_(char* analysis, float* Vdd, float* Rg1, 
                              float* Rg2, float* Rd, float* Rs, 
                              float* Idss, float* Vp, float* rd) {
   // Get inputs of voltage-divider configuration.
   puts("PARAMETERS: ");
   printf("Vdd (V): "); scanf("%f", &(*Vdd));
   printf("Upper Rg (ohm): "); scanf("%f", &(*Rg1));
   printf("Lower Rg (ohm): "); scanf("%f", &(*Rg2));
   printf("Rd (ohm): "); scanf("%f", &(*Rd));
   printf("Rs (ohm): "); scanf("%f", &(*Rs));
   printf("Idss (A): "); scanf("%f", &(*Idss));
   printf("Vp (V): "); scanf("%f", &(*Vp));
   if (strcmp(analysis, "dc")) {
      printf("rd (ohm): "); scanf("%f", &(*rd));
   }
   puts("Calculating results...");
   puts("--------------------------------------------");
}

void _common_gate_inputs_(char* analysis, float* Vdd, float* Vss, 
                          float* Rd, float* Rs, float* Idss, 
                          float* Vp, float* rd) {
   // Get inputs of common-gate configuration.
   puts("PARAMETERS: ");
   printf("Vdd (V): "); scanf("%f", &(*Vdd));
   printf("Vss (V): "); scanf("%f", &(*Vss));
   printf("Rd (ohm): "); scanf("%f", &(*Rd));
   printf("Rs (ohm): "); scanf("%f", &(*Rs));
   printf("Idss (A): "); scanf("%f", &(*Idss));
   printf("Vp (V): "); scanf("%f", &(*Vp));
   if (strcmp(analysis, "dc")) {
      printf("rd (ohm): "); scanf("%f", &(*rd));
   }
   puts("Calculating results ...");
   puts("--------------------------------------------");
}

void _source_follower_inputs_(char* analysis, float* Vdd, float* Vgs,
                              float* Rg, float* Rs, float* Idss, 
                              float* Vp, float* rd) {
   // Get inputs of source-follower configuration.
   puts("PARAMETERS: ");
   printf("Vdd (V): "); scanf("%f", &(*Vdd));
   printf("Vgs (V): "); scanf("%f", &(*Vgs));
   printf("Rg (ohm): "); scanf("%f", &(*Rg));
   printf("Rs (ohm): "); scanf("%f", &(*Rs));
   printf("Idss (A): "); scanf("%f", &(*Idss));
   printf("Vp (V): "); scanf("%f", &(*Vp));
   printf("rd (ohm): "); scanf("%f", &(*rd));
   puts("Calculating results ...");
   puts("--------------------------------------------");
}

void _display_dc_results_(void) {
   // Display the DC results.
   puts("RESULTS: ");
   printf("Id: %f A\n", DCAnalysis.Id);
   printf("Vds: %f A\n", DCAnalysis.Vds);
   printf("Vgs: %f A\n", DCAnalysis.Vgs);
   printf("Vs: %f V\n", DCAnalysis.Vs);
   printf("Vd: %f A\n", DCAnalysis.Vd);
   printf("Vg: %f V\n", DCAnalysis.Vg);
   puts("--------------------------------------------");
}

void _display_ac_results_(void) {
   // Display the AC results.
   puts("RESULTS: ");
   printf("gm: %f S\n", ACAnalysis.gm);
   printf("Zi: %f ohm\n", ACAnalysis.Zi);
   printf("Zo: %f ohm\n", ACAnalysis.Zo);
   printf("Av: %f\n", ACAnalysis.Av);
   printf("Phase: %s\n", ACAnalysis.phase);
   puts("--------------------------------------------");
}
