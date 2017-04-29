set $po = 0
set $pok = 0
set $pov = 0
define ppp
  if $po == 1
	pps $arg0
  else
	if $po == 2
	  pval $arg0
	else
	  if $po == 3
		ps $arg0
	  else
		if $po == 4
		  piptr $arg0
		else 
		  p $arg0
		end
	  end
	end
  end
end

define piptr
  p *(int*)$arg0
end

define ps
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
	p *((SolNfaState*)$arg0)
  else
	p 'null'
  end
end

define precord
  p $arg0
  set $pod = $po
  set $sv = 0
  set $po = $pok
  ppp $arg0->k
  if $argc > 1
	set $sv = $arg1
  end
  if $sv == 0
	set $po = $pov
  	ppp $arg0->v
  end
  set $po = $pod
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
  precords ((SolHash*)$arg0)->size ((SolHash*)$arg0)->records $f
end

define pset
  set $f = 1
  if $argc > 1
	set $f = $arg1
  end
  precords ((SolSet*)$arg0)->hash->size ((SolSet*)$arg0)->hash->records $f 1
end
