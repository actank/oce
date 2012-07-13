
module xce {
module counting {
	dictionary<string, int> CountingData;
	interface CountingManager {
		CountingData getCounting();
		void clearCounting(string type);
	};
};
};

