define pstring
  p (char*)$arg0
end

define precord
  p $arg0
  pstring $arg0->k
  pstring $arg0->v
end

define precords
  set $x = 0
  while ($x < $arg0)
	set $record = (SolHashRecord*)($arg1 + $x)
	precord $record
	set $x = $x + 1
  end
end

define phashrecords
  precords $arg0->size $arg0->records
end
