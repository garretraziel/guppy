#! /bin/sh

if [ $# -eq 0 ]; then
    echo "Je potreba zadat aspon jeden parametr."
    exit 1
fi

for arg in "$@"; do

    if [ ! -f $arg ]; then
	echo "Soubor $arg neexistuje."
	exit 1;
    fi
    
    NAZEV=`mktemp`

    cat <<EOF>$NAZEV
-- Zajisteni zakladni kompatibility IFJ11->Lua
function read(n)
  return io.read(n);
end
function write(...)
  io.write(unpack(arg));
end
function substr(str, i, j)
  if (type(str) ~= "string" or
       type(i) ~= "number" or type(j) ~= "number") then
    return nil;
  end;
  return string.sub(str, i, j);
end
function find (haystack, needle)
  if type(haystack) ~= "string" or type(needle) ~= "string" then
    return nil;
  elseif needle == "" then
    return 0;
  end;
  local found = string.find(haystack, needle);
  if found == nil then
    return false;
  end;
  return found;
end
function sort (s)
  -- Bubble Sort
  local i = 1;
  local j;
  local n;
  local finished;
  local result = s;
  if (type(s) ~= "string") then
    return nil;
  end;
  -- na nasledujicim radku bylo: n = #s; nicmene tak mi to nejelo
  n = string.len(s);
  repeat
    finished = true;
    for j = n, i+1, -1 do
       if (string.byte(result, j-1) > string.byte(result, j)) then
         -- Retezce jsou v Lua nemenitelne (immutable) hodnoty
         result = string.sub(result,1,j-2) .. string.sub(result,j,j) ..
                  string.sub(result,j-1,j-1) .. string.sub(result,j+1, n);
         finished = false;
       end;
    end;
    i = i + 1;
  until (finished or (i == n + 1));
  return result;
end
-- Zde bude nasledovat program jazyka IFJ11
EOF
    cat $arg >> $NAZEV

    export LUA_INIT=@$NAZEV
    lua -e "main()"

    rm $NAZEV
done