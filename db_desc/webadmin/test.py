from tornado import httpserver, ioloop, web
import os

class MainHandler(web.RequestHandler):
  # @web.asynchronous
  def get(self):
    # self.write("Hello, world")
    self.render("about.html", last=1)
    # self.finish()
    
class JsonHandler(web.RequestHandler):
	def get(self):
		self.write('{"sup_id":"44123b23"}')

settings = dict(
  template_path=os.path.join(os.path.dirname(__file__), "templates"),
  static_path=os.path.join(os.path.dirname(__file__), "static"),
  xsrf_cookies=True,
  cookie_secret="11oETzKXQAIaYdkL5iEmIeJJFuYh7EQnp2XdTP1o/Vo=",
  # login_url="/login",
)

testapp = web.Application([
	(r"/", MainHandler),
  (r"/a", JsonHandler),
], **settings)

def main():
  http_server = httpserver.HTTPServer(testapp)
  http_server.listen(8888)
  ioloop.IOLoop.instance().start()

if __name__ == "__main__":
  main()
