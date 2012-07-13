#include <FriendRankUpdateAdapter.h>
using namespace xce::socialgraph;
using namespace MyUtil;

int main(int argc, char* argv[])
{
	if (argc<2)
	{
		fprintf(stderr, "To run, like this: %s 'model' or 'update'\n", argv[0]);
		return -1;
	}

	if (strcmp("model", argv[1])==0)
	{
		int ret = FriendRankUpdateAdapter::instance().TriggerUserModel();
		switch (ret)
		{
			case 0:
				fprintf(stderr, "success.\n");
				break;
			case 1:
				fprintf(stderr, "failure: the last instance is running still.\n");
				break;
			default:
				fprintf(stderr, "failure: unknown error occurred.\n");
				break;
		}
	}
	else if (strcmp("update", argv[1])==0)
	{
		int ret = FriendRankUpdateAdapter::instance().TriggerFriendRank();
		switch (ret)
		{
			case 0:
				fprintf(stderr, "success.\n");
				break;
			case 1:
				fprintf(stderr, "failure: the last instance is running still.\n");
				break;
			case 2:
				fprintf(stderr, "The task cannot be started. need 26214400(KB).\n");
				break;
			default:
				fprintf(stderr, "failure: unknown error occurred.\n");
				break;
		}
	}
	else
	{
		fprintf(stderr, "Error parameter, valid is  'model' or 'update'.\n");
	}
	return 0;
}

