#include <stdio.h>
#include <stdlib.h>
#include <despotify.h>

#include <glib.h>

struct despotify_session *get_session(const char *user, const char *passwd)
{
	struct despotify_session *session;

	if (!despotify_init()) {
		fprintf(stderr, "Can't initialize despotify\n");
		return NULL;
	}

	session = despotify_init_client(NULL, NULL, true, true);
	if (!session) {
		fprintf(stderr, "Can't initialize despotify client\n");
		return NULL;
	}

    if (!despotify_authenticate(session, user, passwd)) {
    	fprintf(stderr, "Can't authenticate despotify session\n");
    	despotify_exit(session);
    	return NULL;
    }

	return session;
}

static void usage(void)
{
	printf("Usage: spotify-playlist-generator outdir user passwd\n");
	exit(1);
}

void output_one_track(FILE *fp, struct ds_track* t)
{
	char url[255];
	char *p;

	p = despotify_track_to_uri(t, url);
	if (!p)
		return;

	fprintf(fp,
			"			<location>spt://%s</location>\n",
			url);

	if (t->has_meta_data) {
		char *creator = g_markup_printf_escaped("%s", t->artist->name);
		char *album = g_markup_printf_escaped("%s", t->album);
		char *title = g_markup_printf_escaped("%s", t->title);

		fprintf(fp,
				"			<creator>%s</creator>\n"
				"			<album>%s</album>\n"
				"			<title>%s</title>\n"
				"			<annotation>Bitrate %u Kbps</annotation>\n",
				creator, album, title, t->file_bitrate / 1000);

		g_free(creator);
		g_free(album);
		g_free(title);
	}
	else
		fprintf(fp, "			<title>UNKNOWN</title>\n");
}

void output_tracks(FILE *fp, struct ds_track* head)
{
	struct ds_track *t;

	for (t = head; t; t = t->next) {
		fprintf(fp, "		<track>\n");
		output_one_track(fp, t);
		fprintf(fp, "		</track>\n");
	}
}

static void output_playlist(const char *outdir, struct ds_playlist* pls)
{
	char buf[1024];
	FILE *fp;

	snprintf(buf, sizeof(buf), "%s/Spotify: %s.xspf", outdir, pls->name);
	fp = fopen(buf, "w");
	if (!fp) {
		fprintf(stderr, "Can't open %s\n", buf);
		exit(1);
	}

	fprintf(fp,
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n"
			"	<trackList>\n");

    output_tracks(fp, pls->tracks);

	fprintf(fp,
			"	</trackList>\n"
			"</playlist>\n");

	fclose(fp);
}

int main(int argc, const char *argv[])
{
	struct despotify_session *session;
	struct ds_playlist *rootlist;
	const char *outdir;
	const char *user;
	const char *passwd;
	struct ds_playlist *p;

	if (argc < 4)
		usage();

	outdir = argv[1];
	user = argv[2];
	passwd = argv[3];

	session = get_session(user, passwd);
	if (!session) {
		fprintf(stderr, "Oh no! Can't get despoitify session\n");
		return 1;
	}

	rootlist = despotify_get_stored_playlists(session);
	for (p = rootlist; p; p = p->next)
		output_playlist(outdir, p);

	despotify_exit(session);

	return 0;
}
