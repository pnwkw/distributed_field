#include "alice_field.h"

#include <AliMagF.h>
#include <glm/gtc/type_ptr.hpp>

mag_field::alice_field::alice_field() : magField(nullptr) {
	magField = createField();
}

glm::vec3 mag_field::alice_field::Field(const glm::vec3 &pos) const noexcept {

	glm::dvec3 const position = pos;
	glm::dvec3 b;

	magField->Field(glm::value_ptr(position), glm::value_ptr(b));

	return b;
}

mag_field::alice_field::~alice_field() {
	delete magField;
}

AliMagF *mag_field::alice_field::createField() {
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

	return AliMagF::CreateFieldMap(TMath::Abs(l3Current) * (l3Polarity ? -1 : 1),
								   TMath::Abs(diCurrent) * (diPolarity ? -1 : 1),
								   polConvention, uniformB, beamEnergy, beamType.Data(), az0, az1);
}
