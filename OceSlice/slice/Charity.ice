module xce
{
	module charity
	{
		interface CharityManager
		{
			int count();
			int next(int begin);
			bool set(int pos, bool flag);
		};
	};
};
