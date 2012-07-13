from feedtpladmin.tests import *

class TestFeedadminController(TestController):

    def test_index(self):
        response = self.app.get(url(controller='FeedAdmin', action='index'))
        # Test response...
