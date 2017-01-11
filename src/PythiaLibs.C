/*
*	Load Libraries
*
*
*
*/
Bool_t PythiaLibs() {
	if (gROOT) {
		cout << "-- Compile and Load the PythiaFluc Libraries --" << endl;
		// check if libraries are already loaded
		if (gClassTable->GetID("GParticle") < 0) {
			gROOT->LoadMacro("~/Dropbox/packard_SeniorProj/moschelli/24Sep2016/src/GParticle.cxx+");
		}
		if (gClassTable->GetID("GEvent") < 0) {
			gROOT->LoadMacro("~/Dropbox/packard_SeniorProj/moschelli/24Sep2016/src/GEvent.cxx+");
		}
		if (gClassTable->GetID("Results") < 0) {
			gROOT->LoadMacro("~/Dropbox/packard_SeniorProj/moschelli/24Sep2016/src/Results.cxx+");
		}
		cout << "-- PythiaFluc Libraries compiled and loaded --" << endl;
	}
	return kTRUE;
}
