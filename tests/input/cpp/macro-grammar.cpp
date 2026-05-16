void sample(bool a,bool b,int fd)
{
if (a) { use(a); }
ef (b) { use(b); }
else { use(fd); }
DEFER { close(fd); };
next(fd);
}
