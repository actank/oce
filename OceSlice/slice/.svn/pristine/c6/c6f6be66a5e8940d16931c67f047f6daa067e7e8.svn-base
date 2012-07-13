#include <Ice/BuiltinSequences.ice>

module xce {
module keepuse {
	dictionary<int, bool> Int2BoolMap;
	interface KeepUseManager {
		void todayuse(int id);
		void use(int days, Int2BoolMap used);
		Int2BoolMap isused(int days, int id);
		bool newday(string date);
		bool canUnset();
		void setCanUnset(bool unset);
	};
};
};

