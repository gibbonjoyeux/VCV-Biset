import processing.svg.*;

boolean	RECORD = false;

PGraphics	p;
int			STEP_Y = 150;	// 150
int			STEP_X = 200;	// 200
int			FEATHER_W = 200;	// 200
int			FEATHER_H = 650;	// 650

float		SCALE_SIZE = 0.01;

void draw_feather(int pos_x, int pos_y) {
	int		x, y;
	int		i;

	p.noStroke();
    for (i = 0; i < 255; ++i) {
        p.fill(i);
        x = pos_x;
        y = int(map(i, 0, 255, pos_y, pos_y - 100));
        p.ellipse(x, y, FEATHER_W, FEATHER_H);
    }
}

void draw_feathering() {
	int        x, y;
	boolean    offset;

    p.beginDraw();
    p.background(75);
    offset = false;
    for (y = height; y > -STEP_Y * 2; y -= STEP_Y) {
        for (x = 0; x < width + STEP_X; x += STEP_X) {
            if (offset)
                draw_feather(x + STEP_X / 2, y);
            else
                draw_feather(x, y);
        }
        offset = !offset;
    }
    p.endDraw();
}

void setup() {
	size(2500, 1000, SVG, "export.svg");
	//size(2500, 1000);
	p = createGraphics(width, height);
	noiseSeed(0);
}

void draw() {
	//float	angle;
	float	size, size_random;
	int		x, y;

	draw_feathering();
	//image(p, 0, 0);

	//noLoop();

	background(14);
    fill(34);
    rectMode(CENTER);
    noStroke();
    for (x = 0; x < width; x += 15) {
        for (y = 0; y < height; y += 15) {
            //angle = noise(x * 0.01, y * 0.01);
            size_random = noise(x * SCALE_SIZE, y * SCALE_SIZE) * 0.5;
			size = 1.0 - red(p.get(x, y)) / 255.0;
            size = map(size, 0, 1, 0.0, 0.5) + size_random;
            //push();
                //translate(x, y);
                //rotate(angle * PI / 2);
                rect(x, y, size * 15, size * 15);
				//circle(0, 0, size * 15);
            //pop();
            //rect(x * STEP + STEP / 2, y * STEP + STEP / 2, size * STEP, size * STEP);
            //circle(x * STEP + STEP / 2, y * STEP + STEP / 2, size * STEP);
        }
    }

	noLoop();
	//save("export.png");
	//exit();
}
