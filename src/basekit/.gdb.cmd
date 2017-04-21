set $po = 0
define ppp
  if $po == 1
	ppp $arg0
  else
	pval $arg0
  end
end

define pstring
  p (char*)$arg0
end

define pval
  if $arg0
    p *(SolVal*)$arg0
  else
	p 'null'
  end
end

define pps
  if $arg0
	pval ((SolPdaState*)$arg0)->s
  else
	p 'null'
  end
end

define precord
  p $arg0
  set $sv = 0
  ppp $arg0->k
  if $argc > 1
	set $sv = $arg1
  end
  if $sv == 0
  	ppp $arg0->v
  end
end

define precords
  set $x = 0
  if $argc > 2
	set $f = $arg2
  else
	set $f = 0
  end
  if $argc > 3
	set $s = $arg3
  else
	set $s = 0
  end
  while ($x < $arg0)
	set $record = (SolHashRecord*)($arg1 + $x)
	if $f == 1
	  if $record->k != 0x0
		precord $record $s
	  end
	else
	  precord $record $s
	end
	set $x = $x + 1
  end
end

define phash
  set $f = 1
  if $argc > 1
	set $f = $arg1
  end
  precords $arg0->size $arg0->records $f
end

define pset
  set $f = 1
  if $argc > 1
	set $f = $arg1
  end
  precords $arg0->hash->size $arg0->hash->records $f 1
end
