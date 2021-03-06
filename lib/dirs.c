/* -*- mode: c; buffer-read-only: t -*- */
#line 2 "./lib/dirs.c.in"
/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <config.h>
#include "dirs.h"
#include <stdlib.h>
#include "ovs-thread.h"
#include "util.h"

struct directory {
    const char *value;          /* Actual value; NULL if not yet determined. */
    const char *default_value;  /* Default value. */
    const char *var_name;       /* Environment variable to override default. */
    struct ovsthread_once once; /* Ensures 'value' gets initialized once. */
};

static const char *
get_dir(struct directory *d)
{
    if (ovsthread_once_start(&d->once)) {
        d->value = getenv(d->var_name);
        if (!d->value || !d->value[0]) {
            d->value = d->default_value;
        }
        ovsthread_once_done(&d->once);
    }
    return d->value;
}

const char *
ovs_sysconfdir(void)
{
    static struct directory d = {
        NULL, "/etc", "OVS_SYSCONFDIR", OVSTHREAD_ONCE_INITIALIZER
    };

    return get_dir(&d);
}

const char *
ovs_pkgdatadir(void)
{
    static struct directory d = {
        NULL, "/usr/share/openvswitch", "OVS_PKGDATADIR", OVSTHREAD_ONCE_INITIALIZER
    };

    return get_dir(&d);
}

const char *
ovs_rundir(void)
{
    static struct directory d = {
        NULL, "/var/run/openvswitch", "OVS_RUNDIR", OVSTHREAD_ONCE_INITIALIZER
    };

    return get_dir(&d);
}

const char *
ovs_logdir(void)
{
    static struct directory d = {
        NULL, "/var/log/openvswitch", "OVS_LOGDIR", OVSTHREAD_ONCE_INITIALIZER
    };

    return get_dir(&d);
}

const char *
ovs_dbdir(void)
{
    static struct ovsthread_once once = OVSTHREAD_ONCE_INITIALIZER;
    static const char *dbdir;

    if (ovsthread_once_start(&once)) {
        dbdir = getenv("OVS_DBDIR");
        if (!dbdir || !dbdir[0]) {
            char *sysconfdir = getenv("OVS_SYSCONFDIR");

            dbdir = (sysconfdir
                     ? xasprintf("%s/openvswitch", sysconfdir)
                     : "/etc/openvswitch");
        }
        ovsthread_once_done(&once);
    }
    return dbdir;
}

const char *
ovs_bindir(void)
{
    static struct directory d = {
        NULL, "/usr/bin", "OVS_BINDIR", OVSTHREAD_ONCE_INITIALIZER
    };

    return get_dir(&d);
}
