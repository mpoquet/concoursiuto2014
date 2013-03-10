import contest

s = contest.Session("", 1)
print s.isValid()
print s.lastError()

s = contest.Session("127.0.0.1", 1080)
print s.isValid()
print s.lastError()

