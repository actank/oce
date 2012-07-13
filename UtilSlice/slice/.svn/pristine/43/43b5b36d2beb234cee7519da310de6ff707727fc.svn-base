module xce
{
module log
{
	sequence<string> StringSeq;

	struct LogEntry{
		StringSeq categories;
		string message;
	};
	["java:type:java.util.ArrayList"] sequence<LogEntry> LogEntries;
	interface Logger{
		void append(LogEntries messages);
	};
};
};
