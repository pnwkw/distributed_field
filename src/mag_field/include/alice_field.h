#ifndef MAG_FIELD_ALICE_FIELD_H
#define MAG_FIELD_ALICE_FIELD_H

#include <glm/glm.hpp>

class AliMagF;

namespace mag_field {
	class alice_field {
	private:
		AliMagF *magField;

	private:
		static AliMagF *createField();

	public:
		alice_field();
		~alice_field();

		glm::vec3 Field(const glm::vec3 &pos) const noexcept;
	};
}// namespace mag_field

#endif//MAG_FIELD_ALICE_FIELD_H
