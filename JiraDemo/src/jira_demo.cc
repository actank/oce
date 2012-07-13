/*
 * =====================================================================================
 *
 *       Filename:  jira_demo.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年03月31日 16时31分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include <util/cpp/AlertClient.h>

int main(int argc, char** argv) {
	try {
		std::cout << "JiraDemo begin" << std::endl;
		MyUtil::AlertClient::alertJira("guanghe.ge", "jira_demo");
		std::cout << "JiraDemo end" << std::endl;
		exit(0);
		return 0;
	} catch (...) {
		std::cout << "jira_demo Exception!" << std::endl;
	}

}
