#include "../AliMagF.h"
#include <TCanvas.h>
#include <TDatabasePDG.h>
#include <TEveTrack.h>
#include <TEveTrackPropagator.h>
#include <TEveVSD.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TPRegexp.h>

#include "json.hpp"

using json = nlohmann::json;

TEveTrackPropagator *trkProp;
TDatabasePDG *database;
AliMagF *field;

AliMagF *createField() {
	Float_t l3Current = 29999.9766f;
	Char_t l3Polarity = 1;
	Float_t diCurrent = 5999.96484f;
	Char_t diPolarity = 1;
	TString beamType = "A-A";
	Float_t beamEnergy = 2510.85571f;

	Int_t polConvention = 0;
	Bool_t uniformB = false;

	int az0 = 208082;
	int az1 = 208082;

	AliMagF *field = AliMagF::CreateFieldMap(TMath::Abs(l3Current) * (l3Polarity ? -1 : 1),
											 TMath::Abs(diCurrent) * (diPolarity ? -1 : 1),
											 polConvention, uniformB, beamEnergy, beamType.Data(), az0, az1);

	return field;
}

json track_to_json(TEveTrack *track, Int_t sign = 0, Int_t comboId = -1, Int_t type = 0) {
	json j_track = json::object();

	j_track["trajectory"] = json::array();
	j_track["field"] = json::array();

	auto px = track->GetMomentum().fX;
	auto py = track->GetMomentum().fY;
	auto pz = track->GetMomentum().fZ;
	j_track["px"] = px;
	j_track["py"] = py;
	j_track["pz"] = pz;
	j_track["particleId"] = int(track->GetPdg());
	j_track["sign"] = sign;
	j_track["comboId"] = comboId;
	j_track["type"] = type;

	Double_t m = 0.0;

	if (track->GetPdg() != 0)
		m = database->GetParticle(track->GetPdg())->Mass();

	j_track["mass"] = m;

	j_track["E"] = TMath::Sqrt(m * m + px * px + py * py + pz * pz);

	std::array<Double_t, 3> point{};
	std::array<Double_t, 3> b{};

	for (Int_t pointI = 0; pointI < track->Size(); pointI++) {
		track->GetPoint(pointI, point[0], point[1], point[2]);

		j_track["trajectory"].push_back(point);

		field->Field(&point[0], &b[0]);

		j_track["field"].push_back(b);
	}

	return j_track;
}

Int_t load_tracks(TTree *fTreeR, json &j) {
	Int_t nTracks = fTreeR->GetEntries();
	std::cerr << "nTracks = " << nTracks << "\n";

	TParticle *p = nullptr;
	fTreeR->SetBranchAddress("Particles", &p);

	Int_t pointsSaved = 0;

	auto tracks = &j["tracks"];
	for (Int_t n = 0; n < nTracks; ++n) {
		fTreeR->GetEntry(n);
		TParticle p2 = *p;
		//p2.Print();
		//std::cout << p2.P() << std::endl;
		if (p2.P() == 0 || p2.P() > 2000 || p2.GetFirstMother() == -1) {
			//std::cerr << "========================= REJECTED ===================\n";
			continue;
		}
		auto track = new TEveTrack(&p2, -1, trkProp);
		track->MakeTrack();

		auto j_track = track_to_json(track);

		if (j_track["trajectory"].size() == 0) {
			//std::cerr << "========================= TRACK IS ZERO ===================\n";
		} else {
			tracks->push_back(j_track);
			pointsSaved += j_track["trajectory"].size();
		}
	}
	return pointsSaved;
}

TObjLink *findEntry(TObjLink *firstLink, TString const &name) {
	TObjLink *lnk = firstLink;
	while (lnk != nullptr) {
		std::cout << lnk->GetObject()->GetName() << std::endl;
		if (lnk->GetObject()->GetName() == name) {
			return lnk;
		}
		lnk = lnk->Next();
	}
	return nullptr;
}

template<typename T>
T *getObjectAtKey(TObject *KeyObj) {
	return dynamic_cast<T *>(dynamic_cast<TKey *>(KeyObj)->ReadObj());
}


void trackData(TString const &file) {
	trkProp = new TEveTrackPropagator();
	trkProp->SetMagField(0.5);
	trkProp->SetMaxR(490);
	database = TDatabasePDG::Instance();
	field = createField();

	auto fDataFile = new TFile(file);
	auto fEvDirKeys = new TObjArray();

	TPMERegexp name_re("Event\\d+");
	TObjLink *lnk = fDataFile->GetListOfKeys()->FirstLink();
	while (lnk != nullptr) {
		//std::cout << lnk->GetObject()->GetName() << std::endl;
		if (name_re.Match(lnk->GetObject()->GetName()) != 0) {
			fEvDirKeys->Add(lnk->GetObject());
		}
		lnk = lnk->Next();
	}

	auto loadEvents = std::min(100, fEvDirKeys->GetEntries());

	Int_t points_total = 0;

	json events = json::array();

	for (Int_t EventIdx = 0; EventIdx < loadEvents; EventIdx++) {
		std::cerr << "Begin Directory tour\n";

		auto Directory = getObjectAtKey<TDirectory>(fEvDirKeys->At(EventIdx));
		std::cerr << "New directory obj ptr " << Directory << std::endl;

		auto TreeK = getObjectAtKey<TTree>(Directory->GetListOfKeys()->FirstLink()->GetObject());

		json j = json::object();
		j["tracks"] = json::array();
		points_total += load_tracks(TreeK, j);

		std::string filename = "out/event_";
		filename = filename + std::to_string(EventIdx) + ".json";

		std::ofstream o(filename);
		o << j << std::endl;

		events.push_back(j);

		printf("POINTS TOTAL: %d\n", points_total);

		if (points_total > 500000) {
			break;
		}
	}

	std::ofstream o("events.json");
	o << events << std::endl;
	o.close();
}

void extract() {
	std::cout << "Loading file" << std::endl;
	trackData("Kinematics.root");
}