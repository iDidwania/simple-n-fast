#include <cstdio>
#include <cstdlib>
#include "dbm.h"
#include "util.h"
#include "perftimer.h"

Logger *TheLogger = 0;

static int
usage(const char *prog)
{
	fprintf(stderr, "%s [-get|-set|-del] -path <db_path> -name <db_name>\n",
			prog);
	fprintf(stderr, "        -key <key> [-value <value>]\n");
	fprintf(stderr, "        [-htsize <hash_table_size>] [-pgsize <page_size>]\n");
	return 1;
}

static int
random_in_range(int seed, int lo, int hi)
{
	if (lo == hi)
		return lo;

	if (seed != 0)
		srand(seed);

	return lo + (rand() % (hi - lo));
}

static void
gen_key_val(char *key, char *val, int len)
{
	int idx;
	char valid_chars[] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r',
		's', 't', 'u', 'v',
		'w', 'x', 'y', 'z'
	};

	for (int i = 0; i < len; i++) {
		idx = random_in_range(0, 0, 36);
		key[i] = valid_chars[idx];
		val[len - i - 1] = toupper(key[i]);
	}

	return;
}

static void
perform_test(Dbm &dbm, long nkeys, int interval)
{
	int retval = OK;
	long j = 0;
	char key[32];
	char val[32];
	PerformanceTimer start, end;

	for (long i = 0; i < nkeys; ++i) {
		if (j == 0) {
			start.record();
		}

		gen_key_val(key, val, 32);
		retval = dbm.set(key, 32, val, 32);
		if (retval != OK) {
			break;
		}

		++j;
		if (j == interval) {
			end.record();
			printf("%ld,%ld\n", j, end - start);
			j = 0;
		}

	}

	if ((retval == OK) && (j > 0)) {
		end.record();
		fprintf(stdout, "%ld,%ld\n", j, end - start);
		fflush(stdout);
		j = 0;
	}

	return;
}

int
main(int argc, const char **argv)
{
	op_t cmd = NIL;
	std::string path;
	std::string name;
	std::string key;
	std::string value;
	int htSize = -1;
	int pgSize = -1;
	int vlen;
	char val[MAX_VALUE_LENGTH + 1];
	char prog[MAXPATHLEN + 1];
	bool testing = false;
	long nkeys = 1000;
	int interval = 100;
	bool verbose = false;

	GetBaseName(prog, MAXPATHLEN + 1, argv[0], true);

	for (int i = 1; i < argc; ++i) {
		if (strcmp("-get", argv[i]) == 0) {
			cmd = GET;
		} else if (strcmp("-set", argv[i]) == 0) {
			cmd = SET;
		} else if (strcmp("-del", argv[i]) == 0) {
			cmd = DEL;
		} else if (strcmp("-test", argv[i]) == 0) {
			testing = true;
		} else if (strcmp("-path", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				path = argv[i];
			} else {
				fprintf(stderr, "missing argument to -path\n");
				return usage(prog);
			}
		} else if (strcmp("-name", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				name = argv[i];
			} else {
				fprintf(stderr, "missing argument to -name\n");
				return usage(prog);
			}
		} else if (strcmp("-key", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				key = argv[i];
			} else {
				fprintf(stderr, "missing argument to -key\n");
				return usage(prog);
			}
		} else if (strcmp("-value", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				value = argv[i];
			} else {
				fprintf(stderr, "missing argument to -value\n");
				return usage(prog);
			}
		} else if (strcmp("-htsize", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				htSize = atoi(argv[i]);
			} else {
				fprintf(stderr, "missing argument to -htsize\n");
				return usage(prog);
			}
		} else if (strcmp("-pgsize", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				pgSize = atoi(argv[i]);
			} else {
				fprintf(stderr, "missing argument to -pgsize\n");
				return usage(prog);
			}
		} else if (strcmp("-n", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				nkeys = atoll(argv[i]);
			} else {
				fprintf(stderr, "missing argument to -n\n");
				return usage(prog);
			}
		} else if (strcmp("-i", argv[i]) == 0) {
			++i;
			if (argv[i]) {
				interval = atoi(argv[i]);
			} else {
				fprintf(stderr, "missing argument to -1\n");
				return usage(prog);
			}
		} else if (strcmp("-v", argv[i]) == 0) {
			verbose = true;
		} else {
			return usage(prog);
		}
	}

	TheLogger = new Logger(path.c_str(), prog, verbose);
	TheLogger->open();

	if (testing) {
		Dbm dbm(path, name);

		if (pgSize != -1)
			dbm.setKeyPageSize(pgSize);

		if (htSize != -1)
			dbm.setHashTableSize(htSize);

		dbm.setMemorySizeInGB(6);

		int retval = dbm.open();
		if (retval == OK) {

			perform_test(dbm, nkeys, interval);

			dbm.close();
		}

		return retval;
	}

	if (cmd == NIL) {
		fprintf(stderr, "one of [-get|-set|-del] must be specified\n");
		return usage(prog);
	}

	if (path.empty()) {
		fprintf(stderr, "database path not specified\n");
		return usage(prog);
	}

	if (name.empty()) {
		fprintf(stderr, "database name not specified\n");
		return usage(prog);
	}

	if (key.empty()) {
		fprintf(stderr, "key not specified\n");
		return usage(prog);
	}

	if ((cmd == SET) && value.empty()) {
		fprintf(stderr, "value not specified\n");
		return usage(prog);
	}

	Dbm dbm(path, name);

	if (pgSize != -1)
		dbm.setKeyPageSize(pgSize);

	if (htSize != -1)
		dbm.setHashTableSize(htSize);

	int retval = dbm.open();
	if (retval == OK) {
		switch (cmd) {
			case GET:
				vlen = MAX_VALUE_LENGTH;
				retval = dbm.get(key.c_str(), key.size(), val, &vlen);
				if (retval == OK) {
					val[vlen] = '\0';
					fprintf(stdout, "%s\n", val);
				} else if (retval == NOT_FOUND) {
					fprintf(stdout, "%s not found\n", key.c_str());
					retval = OK;
				}
				break;

			case SET:
				retval = dbm.set(key.c_str(), key.size(), value.c_str(), value.size());
				if (retval == OK) {
					fprintf(stdout, "%s is set to %s\n",
						key.c_str(), value.c_str());
				}
				break;

			case DEL:
			default:
				retval = dbm.remove(key.c_str(), key.size());
				if (retval == OK) {
					fprintf(stdout, "%s is removed\n",
						key.c_str());
				}
				break;
		}

		dbm.close();
	}

	TheLogger->close();

	if (retval != OK) {
		fprintf(stderr, "Operation failed with status %d\n",
			retval);
		retval = 1;
	}

	return retval;
}