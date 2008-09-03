#!/usr/bin/perl
# $OpenXM$

## asir のプログラムを(できるだけ短い)文字列形式に書き換える.
## usage: asir2string.pl example.rr example.txt
## Bugs: (1) 文字列リテラルの内部も改変されてしまう.
##       (2) トークンの境界を無視する.  例えば
##           「A = B + +C」 が 「A=B++C」となる.

sub main {
	($_) = @_;
	s#/\*.*?\*/##gs ;
	s#\\#\\\\#gs ;
	s#\s+# #gs ;
	s#end[\$;].*$##gs ;
	s#"#\\"#gs ;
	s#\s?([,=;<>\[\]\(\){}\+\-\*/%])\s?#\1#gs ;
	s#^\s?#"#gs ;
	s#\s?$#"#gs ;
	return $_;
}

local $/;

open(STDIN,  $ARGV[0]) if @ARGV > 0 ;
open(STDOUT, '>' . $ARGV[1]) if @ARGV > 1;
my $buffer;
$buffer = <STDIN>;

$buffer = main $buffer;

print $buffer;
