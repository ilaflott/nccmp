#include <log.h>
#include <nccmp_data.h>
#include <nccmp_error.h>
#include <nccmp_group.h>
#include <nccmp_metadata.h>
#include <nccmp_io.h>
#include <nccmp_common.h>
#include <nccmp_var.h>

PRINT_SETUP()

void nccmp_add_group_fullnames(nccmp_group_t* group, nccmp_strlist_t* names)
{
    int i;

    if (!group || !names) {
        return;
    }

    if (! nccmp_exists_in_strlist(names, group->fullname)) {
        nccmp_add_to_strlist(names, group->fullname);
    }

    for(i = 0; i < group->nchildren; ++i) {
        nccmp_add_group_fullnames(group->children[i], names);
    }
}

int nccmp_cmp_group(nccmp_state_t* state, nccmp_group_t* group1, nccmp_group_t* group2)
{
    int status, last_status = EXIT_SUCCESS;
    nccmp_strlist_t* var_names = 0;

    if (!group1 || !group2) {
        if (state->opts.verbose) {
            LOG_INFO("Invalid groups cannot be compared.\n");
        }
        return EXIT_DIFFER;
    }

    if (group1->nchildren != group1->nchildren) {
        if (!state->opts.quiet) {
            PRINT_DIFF(state->opts.debug, "DIFFER : NUMBER OF GROUPS : %zu (\"%s\") <> %zu (\"%s\")\n",
                       group1->nchildren, group1->fullname, group2->nchildren, group2->fullname);
        }
        last_status = EXIT_DIFFER;
        if (!state->opts.force) {
            goto recover;
        }
    }

    if (state->opts.verbose) {
        LOG_INFO("Comparing groups \"%s\" and \"%s\".\n", group1->fullname, group2->fullname);
    }

    status = nccmp_global_atts(& state->opts, group1->id, group2->id);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    if (state->opts.verbose) {
        LOG_INFO("Collecting dimension information for first group.\n");
    }

    nccmp_init_dims(group1->id, & state->dims1, & state->ndims1);
    nccmp_load_dims(group1->id, state->dims1, state->ndims1);

    if (state->opts.verbose) {
        LOG_INFO("Collecting dimension information for second group.\n");
    }

    nccmp_init_dims(group2->id, & state->dims2, & state->ndims2);
    nccmp_load_dims(group2->id, state->dims2, state->ndims2);

    if (state->opts.verbose) {
        LOG_INFO("Collecting variable information for group id = %d.\n", group1->id);
    }

    var_info_init(group1->id, & state->vars1, & state->nvars1);
    var_info_get(group1->id, state->vars1, state->nvars1, state->opts.verbose);
    if (state->opts.debug) {
        nccmp_print_vars(state->vars1, state->nvars1);
    }

    if (state->opts.verbose) {
        LOG_INFO("Found %d variables in first group.\n", state->nvars1);
        LOG_INFO("Collecting variable information for group id = %d.\n", group2->id);
    }

    var_info_init(group2->id, & state->vars2, & state->nvars2);
    var_info_get(group2->id, state->vars2, state->nvars2, state->opts.verbose);
    if (state->opts.debug) {
        nccmp_print_vars(state->vars2, state->nvars2);
    }

    if (state->opts.verbose) {
        LOG_INFO("Found %d variables in second group.\n", state->nvars2);
    }

    var_names = make_cmp_var_list(& state->opts, group1->id, group2->id);

    status = nccmp_cmp_rec_info(state, var_names, group1->id, group2->id);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    if (state->opts.metadata) {
        status = nccmp_cmp_metadata(state, var_names, group1->id, group2->id);
        last_status = status ? status : last_status;
        if (last_status && !state->opts.force) {
            goto recover;
        }
    }

    if (state->opts.encoding) {
        status = nccmp_var_encoding(state, var_names, group1->id, group2->id);
        last_status = status ? status : last_status;
        if (last_status && !state->opts.force) {
            goto recover;
        }
    }

    if (state->opts.data) {
        status = nccmp_data(state, var_names, group1, group2);
        last_status = status ? status : last_status;
        if (last_status && !state->opts.force) {
            goto recover;
        }
    }

recover:
    if (state->opts.verbose) {
      LOG_INFO("Group comparison complete.\n");
    }

    nccmp_free_strlist(& var_names);

    return last_status;
}

int nccmp_compute_num_groups(nccmp_group_t *group)
{
    int result = 0;
    int i;

    if (group) {
        result = 1;
        for(i = 0; i < group->nchildren; ++i) {
            result += nccmp_compute_num_groups(group->children[i]);
        }
    }

    return result;
}

nccmp_group_t* nccmp_create_group()
{
    nccmp_group_t *result = XCALLOC(nccmp_group_t, 1);
    return result;
}

nccmp_group_t* nccmp_create_groups(size_t n)
{
    nccmp_group_t *result = XCALLOC(nccmp_group_t, n);
    return result;
}

nccmp_group_t* nccmp_find_group_by_name(nccmp_group_t* groups, const char* name)
{
    nccmp_group_t* result = 0;
    int i;

    if (! groups) {
        return 0;
    }

    if ( (strcmp(groups->name, name) == 0) ||
         (strcmp(groups->fullname, name) == 0) ) {
        return groups;
    }

    for(i = 0; i < groups->nchildren; ++i)
    {
        result = nccmp_find_group_by_name(groups->children[i], name);
        if (result) {
            return result;
        }
    }

    return 0;
}

nccmp_group_t* nccmp_find_group_by_id(nccmp_group_t* groups, const int id)
{
    nccmp_group_t* result = 0;
    int i;

    if (! groups) {
        return 0;
    }

    if (id == groups->id) {
        return groups;
    }

    for(i = 0; i < groups->nchildren; ++i)
    {
        result = nccmp_find_group_by_id(groups->children[i], id);
        if (result) {
            return result;
        }
    }

    return 0;
}

void nccmp_get_group_ids_recurse(nccmp_group_t *group, int *ids, int *index)
{
    int i;
    if (!group || !ids) {
        return;
    }

    ids[*index] = group->id;
    ++(*index);
    for(i = 0; i < group->nchildren; ++i) {
        nccmp_get_group_ids_recurse(group->children[i], ids, index);
    }
}

void nccmp_get_group_ids(nccmp_group_t *group, int *ids)
{
    int index = 0;
    nccmp_get_group_ids_recurse(group, ids, & index);
}

void nccmp_destroy_group(nccmp_group_t* data)
{
    int i;

    if (! data) {
        return;
    }

    for(i = 0; i < data->nchildren; ++i)
    {
        nccmp_destroy_group(data->children[i]);
        XFREE(data->children[i]);
        data->children[i] = 0;
    }

    if (data->nchildren > 0) {
        XFREE(data->children);
        data->nchildren = 0;
    }
}

void nccmp_destroy_groups(nccmp_group_t** groups, int* ngroups)
{
    int i = 0;
    for(; i < *ngroups; ++i) {
        nccmp_destroy_group(& (*groups)[i]);
    }

    *ngroups = 0;
    XFREE(*groups);
}

int nccmp_get_group_info(int ncid, nccmp_group_t* group) {
    int* gids = 0;
    int i, j, nchildren = 0, status;
    size_t len;

    status = nc_inq_grps(ncid, & nchildren, NULL);
    HANDLE_NC_ERROR(status);

    status = nc_inq_grpname(ncid, group->name);
    HANDLE_NC_ERROR(status);

    status = nc_inq_grpname_full(ncid, & len, group->fullname);
    HANDLE_NC_ERROR(status);

    group->id = ncid;
    group->nchildren = nchildren;

    if (1 > nchildren) {
        goto recover;
    }

    group->children = XMALLOC(nccmp_group_t*, nchildren);
    if (! group->children) {
        LOG_ERROR("Failed to allocate group children for ncid = %d\n", ncid);
        return EXIT_FAILED;
    }

    gids = XCALLOC(int, nchildren);
    if (! gids) {
        LOG_ERROR("Failed to allocate group children ids for ncid = %d\n", ncid);
        XFREE(group->children);
        return EXIT_FAILED;
    }

    status = nc_inq_grps(ncid, NULL, gids);
    HANDLE_NC_ERROR(status);

    for (i = 0; i < nchildren; ++i) {
        group->children[i] = nccmp_create_group();
        if (! group->children[i]) {
            LOG_ERROR("Failed to allocate group child for id = %d\n", gids[i]);
            status = EXIT_FAILED;
            goto recover;
        }

        group->children[i]->parent = group;
        status = nccmp_get_group_info(gids[i], group->children[i]);
        if (status) {
            for(j = 0; j <= i; ++j) {
                XFREE(group->children[j]);
            }
            XFREE(group->children);
            goto recover;
        }
    }

recover:
    XFREE(gids);

    return status;
}

int nccmp_cmp_groups(nccmp_state_t* state)
{
    int i, status = EXIT_SUCCESS, last_status = EXIT_SUCCESS;
    nccmp_group_t *child1, *child2;
    nccmp_strlist_t* tmp_group_names = NULL;
    nccmp_strlist_t* group_names_to_process = NULL;

    if (state->opts.groupnames->size) {
        group_names_to_process = state->opts.groupnames;
    } else {
        tmp_group_names = nccmp_new_strlist(NC_MAX_VARS);
        nccmp_add_group_fullnames(state->thread_groups1, tmp_group_names);
        nccmp_add_group_fullnames(state->thread_groups2, tmp_group_names);
        group_names_to_process = tmp_group_names;
    }

    for(i = 0; i < group_names_to_process->size; ++i) {
        child1 = nccmp_find_group_by_name(state->thread_groups1, group_names_to_process->items[i]);
        if (! child1) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : Failed to find group %s in file \"%s\"\n",
                           group_names_to_process->items[i], state->opts.file1);
            }
            last_status = EXIT_DIFFER;
            if (!state->opts.force) {
                goto recover;
            }
        }
        child2 = nccmp_find_group_by_name(state->thread_groups2, group_names_to_process->items[i]);
        if (! child2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : Failed to find group %s in file \"%s\"\n",
                           group_names_to_process->items[i], state->opts.file2);
            }
            last_status = EXIT_DIFFER;
            if (!state->opts.force) {
                goto recover;
            }
        }

        status = nccmp_cmp_group(state, child1, child2);
        last_status = status ? status : last_status;
        if (last_status && !state->opts.force) {
            goto recover;
        }
    }

recover:
    nccmp_free_strlist(& tmp_group_names);

    return last_status;
}
