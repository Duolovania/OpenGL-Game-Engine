#pragma once
#include "test.h"

namespace testSpace
{
	class TestClearColour : public Test
	{
		public:
			TestClearColour();
			~TestClearColour();

			void OnUpdate(float deltaTime) override;
			void OnImGuiRender() override;
			void OnRender(glm::mat4 proj) override;

		private:
			float clearColour[4];
	};
}
