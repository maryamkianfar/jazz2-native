#pragma once

#include "IInputManager.h"

#include <Containers/SmallVector.h>
#include <Containers/StringView.h>

using namespace Death::Containers;

namespace nCine
{
	class IInputManager;
	class JoyButtonEvent;
	class JoyAxisEvent;
	class JoyConnectionEvent;
	class IInputEventHandler;
	template <class T> class Vector2;
	using Vector2f = Vector2<float>;

	class JoyMapping
	{
	public:
		JoyMapping();
		~JoyMapping() {}

		void init(const IInputManager* inputManager);
		inline void setHandler(IInputEventHandler* inputEventHandler) {
			inputEventHandler_ = inputEventHandler;
		}

		bool addMappingFromString(const char* mappingString);
		void addMappingsFromStrings(const char** mappingStrings);
		void addMappingsFromFile(const StringView& path);
		inline unsigned int numMappings() const {
			return (unsigned int)mappings_.size();
		}

		void onJoyButtonPressed(const JoyButtonEvent& event);
		void onJoyButtonReleased(const JoyButtonEvent& event);
		void onJoyHatMoved(const JoyHatEvent& event);
		void onJoyAxisMoved(const JoyAxisEvent& event);
		bool onJoyConnected(const JoyConnectionEvent& event);
		void onJoyDisconnected(const JoyConnectionEvent& event);

		bool isJoyMapped(int joyId) const;
		const JoyMappedState& joyMappedState(int joyId) const;
		void deadZoneNormalize(Vector2f& joyVector, float deadZoneValue = IInputManager::LeftStickDeadZone) const;
		static JoystickGuid createJoystickGuid(uint16_t bus, uint16_t vendor, uint16_t product, uint16_t version, const StringView& name, uint8_t driverSignature, uint8_t driverData);
		
		int findMappingByGuid(const JoystickGuid& guid) const;
		int findMappingByName(const char* name) const;

	private:
		static constexpr std::uint32_t MaxNameLength = 64;

		struct MappingDescription
		{
			struct Axis
			{
				AxisName name;
				ButtonName buttonNamePositive;
				ButtonName buttonNameNegative;
				float min;
				float max;
			};

			static const int MaxNumAxes = 10;
			static const int MaxNumButtons = 34;
			static const int MaxHatButtons = 4; // The four directions

			Axis axes[MaxNumAxes];
			/// Button axes (buttons mapped as axes, like when analog triggers are missing)
			AxisName buttonAxes[MaxNumAxes];
			ButtonName buttons[MaxNumButtons];
			ButtonName hats[MaxHatButtons];

			MappingDescription();
		};

		struct MappedJoystick
		{
			JoystickGuid guid;
			char name[MaxNameLength];
			MappingDescription desc;

			MappedJoystick();
		};

		struct AssignedMapping
		{
			bool isValid;
			MappingDescription desc;
		};

		static const char* AxesStrings[];
		static const char* ButtonsStrings[];

		static const int MaxNumJoysticks = 4;
		SmallVector<MappedJoystick, 0> mappings_;
		AssignedMapping assignedMappings_[MaxNumJoysticks];

		static JoyMappedState nullMappedJoyState_;
		static SmallVector<JoyMappedState, MaxNumJoysticks> mappedJoyStates_;
		static JoyMappedButtonEvent mappedButtonEvent_;
		static JoyMappedAxisEvent mappedAxisEvent_;

		const IInputManager* inputManager_;
		IInputEventHandler* inputEventHandler_;

		void checkConnectedJoystics();
		bool parseMappingFromString(const char* mappingString, MappedJoystick& map);
		bool parsePlatformKeyword(const char* start, const char* end) const;
		bool parsePlatformName(const char* start, const char* end) const;
		int parseAxisName(const char* start, const char* end) const;
		int parseButtonName(const char* start, const char* end) const;
		int parseAxisMapping(const char* start, const char* end, MappingDescription::Axis& axis) const;
		int parseButtonMapping(const char* start, const char* end) const;
		int parseHatMapping(const char* start, const char* end) const;
		int hatStateToIndex(unsigned char hatState) const;
		void trimSpaces(const char** start, const char** end) const;
	};

}
