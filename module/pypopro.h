/*
 * Copyright (C) 2014 Boris Grozev <boris@jitsi.org>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <Python.h>
#include "decoder.h"
#include "encoder.h"
#include "overlay.h"

//Decoder methods
static PyObject* py_pypopro_decoder_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_decoder_read(PyObject *self, PyObject *args);
static PyObject* py_pypopro_decoder_close(PyObject *self, PyObject *args);

//Encoder methods
static PyObject* py_pypopro_encoder_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_encoder_add_frame(PyObject *self, PyObject *args);
static PyObject* py_pypopro_encoder_close(PyObject *self, PyObject *args);

//Scaler methods
static PyObject* py_pypopro_scaler_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_scaler_scale(PyObject *self, PyObject *args);
static PyObject* py_pypopro_scaler_close(PyObject *self, PyObject *args);

//Overlayer methods
static PyObject* py_pypopro_overlayer_init(PyObject *self, PyObject *args);
static PyObject* py_pypopro_overlayer_overlay(PyObject *self, PyObject *args);
static PyObject* py_pypopro_overlayer_close(PyObject *self, PyObject *args);

//FFMPEG wrappers
static PyObject* py_pypopro_get_dimensions(PyObject *self, PyObject *args);
