#include <bits/stdc++.h>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define OTP_DEB false
#define ifdeb if (OTP_DEB)
#define debug if (OTP_DEB) cout
#define vec vector
#define str string
#define fst first
#define snd second

int coincides(unsigned char* a, unsigned char* b, int channels) {
	if (a == b) return true;
	for (int i = 0; i < channels; i++)
		if (*(a+i) != *(b+i)) return false;
	return true;
}

void bfs(
	unsigned char* img, int w, int h, int c, 
	vec <pair <int, int>> &shape, 
	vec <str> &color, 
	int &snum
) {
	color.push_back(str(' ', c));
	shape.push_back({});
	snum = 0;
	
	vec <vec <bool>> was(h, vec<bool>(w, false));
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {

			if (was[i][j]) continue;
			shape.push_back({i, j});

			str current_col = "";
			for (int k = 0; k < c; k++) 
				current_col.push_back(char(*(img+(i*w+j)*c+k)));
			
			color.push_back(current_col);
			snum++;

			queue <pair <int, int>> q; q.push({i, j});
			while (q.size()) {
				auto fr = q.front(); q.pop();
				if (fr.fst >= h || fr.snd >= w || min(fr.fst, fr.snd) < 0)
					continue;
				if (
					was[fr.fst][fr.snd] > 0 || 
					!coincides(img+(i*w+j)*c, img+(fr.fst*w+fr.snd)*c, c)
				) continue;
				
				was[fr.fst][fr.snd] = true; 

				q.push({fr.fst+1, fr.snd});
				q.push({fr.fst-1, fr.snd});
				q.push({fr.fst, fr.snd+1});
				q.push({fr.fst, fr.snd-1});
			}
		}
	}
}

str colorize(str col) {
	
	int col_chan = min(3, (int)col.size()); 
	map <int, int> mp;

	vec <unsigned char> ucv;
	for (char c : col)
		ucv.push_back((unsigned char)(c));

	str hex = "";
	for (int i = 0; i < col_chan; i++) {
		hex += string({
			char((ucv[i]/16 < 10) ? '0'+ucv[i]/16 : 'a'+ucv[i]/16-10),
			char((ucv[i]%16 < 10) ? '0'+ucv[i]%16 : 'a'+ucv[i]%16-10)
		});
	}

	if (col.size() == 1)
		hex = hex + hex + hex;

	str res = "fill=\"#" + hex + "\"";
	if (col.size() > 3 && ucv[3] != 255) 
		res += " fill-opacity=\"" + to_string(((float)(ucv[3]))/255.0) + "\"";
	
	return res;
}

str draw(unsigned char* img, int w, int h, int c, pair <int, int> o, int dir) {

	debug << "--------\n";

	str res = "", path = "";
	set <pair <int, int>> border;

	vec <vec <bool>> was(h, vec<bool>(w));
	queue <pair <int, int>> q;
	q.push(o);

	while (q.size()) {
		auto fr = q.front(); q.pop();
		debug << "(" << fr.fst << ", " << fr.snd << ")\n";

		if (fr.fst >= h || fr.snd >= w || min(fr.fst, fr.snd) < 0)
			continue;
		if (
			was[fr.fst][fr.snd] > 0 || 
			!coincides(img+(o.fst*w+o.snd)*c, img+(fr.fst*w+fr.snd)*c, c)
		) continue;

		was[fr.fst][fr.snd] = true; 

		for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			pair <int, int> n = {fr.fst+i-1, fr.snd+j-1};
			if (
				n.fst >= h || n.snd >= w || min(n.fst, n.snd) < 0 || 
				!coincides(img+(fr.fst*w+fr.snd)*c, img+(n.fst*w+n.snd)*c, c)
			) {
				if ((i-1)*(j-1)!=0) 
					border.insert({3*fr.fst+i, 3*fr.snd+j});
				else {
					for (int k = 0; k < 3; k++) {
						if (i == 0) border.insert({3*fr.fst+0, 3*fr.snd+k}); else
						if (i == 2) border.insert({3*fr.fst+2, 3*fr.snd+k}); else
						if (j == 0) border.insert({3*fr.fst+k, 3*fr.snd+0}); else
						if (j == 2) border.insert({3*fr.fst+k, 3*fr.snd+2});
					}
				}
			}
		}
		
		q.push({fr.fst+1, fr.snd});
		q.push({fr.fst-1, fr.snd});
		q.push({fr.fst, fr.snd+1});
		q.push({fr.fst, fr.snd-1});
		
	}

	bool once = false;
	
	while (border.size()) {
		debug << "........\n";
	
		pair <int, int> p = *border.begin(), pr; border.erase(p);
		res += "M" + to_string((p.snd+1)/3) + " " + to_string((p.fst+1)/3);
		debug << "(" << p.fst << ", " << p.snd << ")\n";
		p.fst += dir%2; p.snd += (dir+1)%2;
		
		for(;;) {
			
			debug << "(" << p.fst << ", " << p.snd << ")\n";
			border.erase(p); pr = p;
			if (border.find({p.fst, p.snd-1}) != border.end()) p = {p.fst, p.snd-1}; else
			if (border.find({p.fst+1, p.snd}) != border.end()) p = {p.fst+1, p.snd}; else
			if (border.find({p.fst, p.snd+1}) != border.end()) p = {p.fst, p.snd+1}; else
			if (border.find({p.fst-1, p.snd}) != border.end()) p = {p.fst-1, p.snd}; else
			break;
			pair <int, int> diff = { 
				(p.fst+1)/3 - (pr.fst+1)/3, 
				(p.snd+1)/3 - (pr.snd+1)/3
			};
			if (diff.fst ==  1) path += "u";
			if (diff.fst == -1) path += "d";
			if (diff.snd ==  1) path += "r";
			if (diff.snd == -1) path += "l";
		}

		int len = 1;
		for (int i = 1; i < (int)path.size(); i++) {
			if (path[i] != path[i-1]) {
				switch (path[i-1]) {
					case 'u': res += "v"  + to_string(len); break;
					case 'd': res += "v-" + to_string(len); break;
					case 'r': res += "h"  + to_string(len); break;
					case 'l': res += "h-" + to_string(len); break;
				}
				len = 0;
			}
			len++;
		}
		
		path = ""; res += "z";

		if (!once) dir++, once = true;
	}

	return res;
}

int main(int argc, char** argv) {
	
	if (argc != 2) {
		cout << "invalid arguments\n";
		return 0;
	}
	
	int w, h, c, snum;
	unsigned char* img = stbi_load(argv[1], &w, &h, &c, 0);
	
	vec <pair <int, int>> shape;
	vec <str> color;

	str res = "<svg width=\"" + to_string(w) + "\" height=\"" + 
	to_string(h) +"\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n";
	
	bfs(img, w, h, c, shape, color, snum);

	map <str, str> colors;
	for (int i = 1; i <= snum; i++) 
		colors[color[i]] = "<path " + colorize(color[i]) + " d=\"";

	for (int i = 1; i <= snum; i++) {
		str col = color[i];
		colors[col] += draw(img, w, h, c, shape[i], 1);
	}
	
	for (auto x : colors) res += x.snd + "\"/>\n";

	res += "</svg>";
	cout << res << "\n";
	
	stbi_image_free(img);
	
}
