#include "clar_libgit2.h"
#include "apply_helpers.h"

static git_repository *repo;

#define TEST_REPO_PATH "merge-recursive"

void test_apply_both__initialize(void)
{
	git_oid oid;
	git_commit *commit;

	repo = cl_git_sandbox_init(TEST_REPO_PATH);

	git_oid_fromstr(&oid, "539bd011c4822c560c1d17cab095006b7a10f707");
	cl_git_pass(git_commit_lookup(&commit, repo, &oid));
	cl_git_pass(git_reset(repo, (git_object *)commit, GIT_RESET_HARD, NULL));
	git_commit_free(commit);
}

void test_apply_both__cleanup(void)
{
	cl_git_sandbox_cleanup();
}

void test_apply_both__generated_diff(void)
{
	git_oid a_oid, b_oid;
	git_commit *a_commit, *b_commit;
	git_tree *a_tree, *b_tree;
	git_diff *diff;
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;

	struct merge_index_entry both_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t both_expected_cnt = sizeof(both_expected) /
		sizeof(struct merge_index_entry);

	git_oid_fromstr(&a_oid, "539bd011c4822c560c1d17cab095006b7a10f707");
	git_oid_fromstr(&b_oid, "7c7bf85e978f1d18c0566f702d2cb7766b9c8d4f");
	cl_git_pass(git_commit_lookup(&a_commit, repo, &a_oid));
	cl_git_pass(git_commit_lookup(&b_commit, repo, &b_oid));

	cl_git_pass(git_commit_tree(&a_tree, a_commit));
	cl_git_pass(git_commit_tree(&b_tree, b_commit));

	cl_git_pass(git_diff_tree_to_tree(&diff, repo, a_tree, b_tree, &diff_opts));
	cl_git_pass(git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, both_expected, both_expected_cnt);
	validate_apply_workdir(repo, both_expected, both_expected_cnt);

	git_diff_free(diff);
	git_tree_free(a_tree);
	git_tree_free(b_tree);
	git_commit_free(a_commit);
	git_commit_free(b_commit);
}

void test_apply_both__parsed_diff(void)
{
	git_diff *diff;

	struct merge_index_entry both_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t both_expected_cnt = sizeof(both_expected) /
		sizeof(struct merge_index_entry);

	cl_git_pass(git_diff_from_buffer(&diff,
		DIFF_MODIFY_TWO_FILES, strlen(DIFF_MODIFY_TWO_FILES)));
	cl_git_pass(git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, both_expected, both_expected_cnt);
	validate_apply_workdir(repo, both_expected, both_expected_cnt);

	git_diff_free(diff);
}

void test_apply_both__removes_file(void)
{
	git_diff *diff;

	struct merge_index_entry both_expected[] = {
		{ 0100644, "f51658077d85f2264fa179b4d0848268cb3475c3", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "94d2c01087f48213bd157222d54edfefd77c9bba", 0, "veal.txt" },
	};
	size_t both_expected_cnt = sizeof(both_expected) /
	    sizeof(struct merge_index_entry);

	cl_git_pass(git_diff_from_buffer(&diff, DIFF_DELETE_FILE,
		strlen(DIFF_DELETE_FILE)));
	cl_git_pass(git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, both_expected, both_expected_cnt);
	validate_apply_workdir(repo, both_expected, both_expected_cnt);

	git_diff_free(diff);
}

void test_apply_both__adds_file(void)
{
	git_diff *diff;

	struct merge_index_entry both_expected[] = {
		{ 0100644, "f51658077d85f2264fa179b4d0848268cb3475c3", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "6370543fcfedb3e6516ec53b06158f3687dc1447", 0, "newfile.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "94d2c01087f48213bd157222d54edfefd77c9bba", 0, "veal.txt" },
	};
	size_t both_expected_cnt = sizeof(both_expected) /
	    sizeof(struct merge_index_entry);

	cl_git_pass(git_diff_from_buffer(&diff,
		DIFF_ADD_FILE, strlen(DIFF_ADD_FILE)));
	cl_git_pass(git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, both_expected, both_expected_cnt);
	validate_apply_workdir(repo, both_expected, both_expected_cnt);

	git_diff_free(diff);
}

void test_apply_both__application_failure_leaves_index_unmodified(void)
{
	git_diff *diff;
	git_index *index;

	const char *diff_file = DIFF_MODIFY_TWO_FILES;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "f51658077d85f2264fa179b4d0848268cb3475c3", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
		sizeof(struct merge_index_entry);

	/* mutate the index */
	cl_git_pass(git_repository_index(&index, repo));
	cl_git_pass(git_index_remove(index, "veal.txt", 0));
	cl_git_pass(git_index_write(index));
	git_index_free(index);

	cl_git_pass(git_diff_from_buffer(&diff, diff_file, strlen(diff_file)));
	cl_git_fail_with(GIT_EAPPLYFAIL, git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_workdir_unchanged(repo);

	git_diff_free(diff);
}

void test_apply_both__index_must_match_workdir(void)
{
	git_diff *diff;
	git_index *index;
	git_index_entry idx_entry;

	const char *diff_file = DIFF_MODIFY_TWO_FILES;

	/*
	 * Append a line to the end of the file in both the index and the
	 * working directory.  Although the appended line would allow for
	 * patch application in each, the line appended is different in
	 * each, so the application should not be allowed.
	 */
	cl_git_append2file("merge-recursive/asparagus.txt",
	    "This is a modification.\n");

	cl_git_pass(git_repository_index(&index, repo));

	memset(&idx_entry, 0, sizeof(git_index_entry));
	idx_entry.mode = 0100644;
	idx_entry.path = "asparagus.txt";
	cl_git_pass(git_oid_fromstr(&idx_entry.id, "06d3fefb8726ab1099acc76e02dfb85e034b2538"));
	git_index_add(index, &idx_entry);

	cl_git_pass(git_index_write(index));
	git_index_free(index);

	cl_git_pass(git_diff_from_buffer(&diff, diff_file, strlen(diff_file)));
	cl_git_fail_with(GIT_EAPPLYFAIL, git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	git_diff_free(diff);
}

void test_apply_both__application_failure_leaves_workdir_unmodified(void)
{
	git_diff *diff;
	git_index *index;

	const char *diff_file = DIFF_MODIFY_TWO_FILES;

	struct merge_index_entry workdir_expected[] = {
		{ 0100644, "f51658077d85f2264fa179b4d0848268cb3475c3", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "8684724651336001c5dbce74bed6736d2443958d", 0, "veal.txt" },
	};
	size_t workdir_expected_cnt = sizeof(workdir_expected) /
		sizeof(struct merge_index_entry);

	/* mutate the workdir */
	cl_git_rewritefile("merge-recursive/veal.txt",
	    "This is a modification.\n");

	cl_git_pass(git_repository_index(&index, repo));
	git_index_add_bypath(index, "veal.txt");
	cl_git_pass(git_index_write(index));
	git_index_free(index);

	cl_git_pass(git_diff_from_buffer(&diff, diff_file, strlen(diff_file)));
	cl_git_fail_with(GIT_EAPPLYFAIL, git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_workdir(repo, workdir_expected, workdir_expected_cnt);

	git_diff_free(diff);
}

void test_apply_both__keeps_nonconflicting_changes(void)
{
	git_diff *diff;
	git_index *index;
	git_index_entry idx_entry;

	const char *diff_file = DIFF_MODIFY_TWO_FILES;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "898d12687fb35be271c27c795a6b32c8b51da79e", 0, "beef.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
		sizeof(struct merge_index_entry);

	struct merge_index_entry workdir_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
        { 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
        { 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "f75ba05f340c51065cbea2e1fdbfe5fe13144c97", 0, "gravy.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t workdir_expected_cnt = sizeof(workdir_expected) /
		sizeof(struct merge_index_entry);

	/* mutate the index */
	cl_git_pass(git_repository_index(&index, repo));

	memset(&idx_entry, 0, sizeof(git_index_entry));
	idx_entry.mode = 0100644;
	idx_entry.path = "beef.txt";
	cl_git_pass(git_oid_fromstr(&idx_entry.id, "898d12687fb35be271c27c795a6b32c8b51da79e"));
	git_index_add(index, &idx_entry);

	cl_git_pass(git_index_remove(index, "bouilli.txt", 0));
	cl_git_pass(git_index_write(index));
	git_index_free(index);

	/* and mutate the working directory */
    cl_git_rmfile("merge-recursive/oyster.txt");
    cl_git_rewritefile("merge-recursive/gravy.txt", "Hello, world.\n");

	cl_git_pass(git_diff_from_buffer(&diff, diff_file, strlen(diff_file)));
	cl_git_pass(git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_apply_workdir(repo, workdir_expected, workdir_expected_cnt);

	git_diff_free(diff);
}

void test_apply_both__can_apply_nonconflicting_file_changes(void)
{
	git_diff *diff;
	git_index *index;

	const char *diff_file = DIFF_MODIFY_TWO_FILES;

	struct merge_index_entry both_expected[] = {
		{ 0100644, "f8a701c8a1a22c1729ee50faff1111f2d64f96fc", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t both_expected_cnt = sizeof(both_expected) /
		sizeof(struct merge_index_entry);

	/*
	 * Replace the workdir file with a version that is different than
	 * HEAD but such that the patch still applies cleanly.  This item
	 * has a new line appended.
	 */
	cl_git_append2file("merge-recursive/asparagus.txt",
	    "This line is added in the index and the workdir.\n");

	cl_git_pass(git_repository_index(&index, repo));
	git_index_add_bypath(index, "asparagus.txt");
	cl_git_pass(git_index_write(index));
	git_index_free(index);

	cl_git_pass(git_diff_from_buffer(&diff, diff_file, strlen(diff_file)));
	cl_git_pass(git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, both_expected, both_expected_cnt);
	validate_apply_workdir(repo, both_expected, both_expected_cnt);

	git_diff_free(diff);
}

void test_apply_both__honors_crlf_attributes(void)
{
	git_diff *diff;
	git_oid oid;
	git_commit *commit;

	const char *diff_file = DIFF_MODIFY_TWO_FILES;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
		sizeof(struct merge_index_entry);

	struct merge_index_entry workdir_expected[] = {
		{ 0100644, "176a458f94e0ea5272ce67c36bf30b6be9caf623", 0, ".gitattributes" },
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t workdir_expected_cnt = sizeof(workdir_expected) /
		sizeof(struct merge_index_entry);

	cl_git_mkfile("merge-recursive/.gitattributes", "* text=auto\n");

	cl_git_rmfile("merge-recursive/asparagus.txt");
	cl_git_rmfile("merge-recursive/veal.txt");

	git_oid_fromstr(&oid, "539bd011c4822c560c1d17cab095006b7a10f707");
	cl_git_pass(git_commit_lookup(&commit, repo, &oid));
	cl_git_pass(git_reset(repo, (git_object *)commit, GIT_RESET_HARD, NULL));
	git_commit_free(commit);

	cl_git_pass(git_diff_from_buffer(&diff, diff_file, strlen(diff_file)));
	cl_git_pass(git_apply(repo, diff, GIT_APPLY_LOCATION_BOTH, NULL));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_apply_workdir(repo, workdir_expected, workdir_expected_cnt);

	git_diff_free(diff);
}
