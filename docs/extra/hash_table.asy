size(800);
defaultpen(linewidth(1.5pt)+fontsize(20pt));

picture node(int x, int y, int state = 4, bool win = false){
	picture pic;
	draw(pic,box((0,0),(5,4)));
	draw(pic,box((0.5,1.5),(3.5,3.5)));
	for(int i=0;i<6;++i){
		if(state%2>=1){
			draw(pic,circle((1+i%3,2+(i>=3?1:0)),0.5),p=blue);
		}
		state = floor(state/2);
	}
	draw(pic,box((4,2),(5,3)),p=red);
	draw(pic,box((1,0),(2,1)),p=red);
	draw(pic,box((2,0),(3,1)),p=red);
	draw(pic,box((3,0),(4,1)),p=red);
	if(win) label(pic,"WIN",(2,2.5));

	return shift((x*8,y*5))*pic;
}

picture arrowr(int x, int y, bool isnull=false){
	picture pic;
	path pa = (-3.5+x*8,2.5+y*5)--(0+x*8,2.5+y*5);
	if(isnull){
		draw(pic,pa,Bar);
	}else{
		draw(pic,pa,Arrow);
	}
	return pic;
}

for(int i=0;i<6;++i){
	draw(box((-4,5*i),(-3,5*(i+1))),p=red);
}

// Draw hash_table:
add(node(0,0,9));
add(arrowr(0,0));

add(arrowr(0,1));
add(node(0,1,33));
add(arrowr(1,1));
add(node(1,1,5));
add(arrowr(2,1,true));

add(arrowr(0,2));
add(node(0,2,6,true));
add(arrowr(1,2,true));

add(arrowr(0,3,true));

add(arrowr(0,4));
add(node(0,4,17));
add(arrowr(1,4,true));

add(arrowr(0,5,true));

// Draw graph:
add(node(5,5,17));
draw((1.5+5*8,0.5+5*5)--(2.5+4*8,3*5+4),Arrow);
draw((2.5+5*8,0.5+5*5)--(2.5+6*8,3*5+4),Arrow);

add(node(4,3,5));
draw((1.5+4*8,0.5+3*5)--(2.5+4*8,1*5+4),Arrow);
draw((2.5+4*8,0.5+3*5)--(2.5+6*8,1*5+4),Arrow);

add(node(6,3,33));
draw((1.5+6*8,0.5+3*5)--(2.5+6*8,1*5+4),Arrow);

add(node(4,1,9));
draw((1.5+4*8,0.5+1*5){0.5S}..(4*8,4*5)..(0+5*8,5*5+2){0.5E},Arrow);
draw((2.5+4*8,0.5+1*5){0.5S}..(0+6*8,1*5+2){0.5E},Arrow);

add(node(6,1,6,true));
