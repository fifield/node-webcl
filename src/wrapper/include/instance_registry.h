/*
 * This file is part of WebCL – JavaScript bindings for OpenCL
 * http://webcl.nokiaresearch.com/
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Jari Nikara  ;jari.nikara@nokia.com;
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 *
 * The package is based on a published Khronos OpenCL 1.1 Specification,
 * see http://www.khronos.org/opencl/.
 *
 * OpenCL is a trademark of Apple Inc.
 */

/** \file instance_registry.h
 * InstanceRegistry class definition.
 */

#ifndef _INSTANCE_REGISTRY_H_
#define _INSTANCE_REGISTRY_H_

#include <map>


/** This template class provides an instance registry for WebCL classes.
 * Many WebCL classes contain some internal information that provides the actual
 * identity of the instance in the form of a reference to some external entity.
 * In certain occasions it is necessary to find a WebCL instance by the bare
 * external identity reference that may come from e.g. an external library.
 */
template <class Tid, class Tinstance>
class InstanceRegistry {
  public:
    InstanceRegistry () : mInstances () { }
    ~InstanceRegistry () { }

    bool add (Tid aId, Tinstance aInstance) {
      return mInstances.insert (std::make_pair (aId, aInstance)).second;
    }

    bool remove (Tid aId) {
      return mInstances.erase (aId) != 0;
    }

    bool findById (Tid aId, Tinstance* aInstance) {
      if (!aInstance)
        return false;
      typename std::map<Tid, Tinstance>::iterator i = mInstances.find (aId);
      if (i != mInstances.end ()) {
        *aInstance = i->second;
        return true;
      }
      return false;
    }

  private:
    /// Copying is not allowed.
    InstanceRegistry (InstanceRegistry const&);
    /// Assignment is not allowed.
    InstanceRegistry& operator= (InstanceRegistry const&);

    std::map <Tid, Tinstance> mInstances;
};

#endif // _INSTANCE_REGISTRY_H_
