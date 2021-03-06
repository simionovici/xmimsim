#include "xmi_xml.h"


int main(int argc, char *argv[]) {

	char xmso_file[] = "nist.xmso";
	struct xmi_input *input;
	struct xmi_fluorescence_line_counts *brute_force_history;
	int nbrute_force_history;

	struct xmi_fluorescence_line_counts *var_red_history;
	int nvar_red_history;

	double **channels_conv;
	double **channels_unconv;
	int nchannels;
	
	char *xmsi_file;
	int use_zero_interactions;
	int n_interactions;

	xmi_xmlLoadCatalog();

	xmi_read_output_xml(	xmso_file,
				&input,
				&brute_force_history,
				&nbrute_force_history,
				&var_red_history,
				&nvar_red_history,
				&channels_conv,
				&channels_unconv,
				&nchannels,
				&n_interactions,
				&xmsi_file,
				&use_zero_interactions);



	return 0;
}


