/* 
**    Copyright (C) 2024 Quectel Wireless Solutions Co., Ltd.
**
**    This program is free software; you can redistribute it and/or
**    modify it under the terms of the GNU General Public License
**    as published by the Free Software Foundation; either version 2
**    of the License, or (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, see
**    <https://www.gnu.org/licenses/>.
**
**    //device/system/reference-ril/at_tok.h
**
**    Copyright 2006, The Android Open Source Project
**
**    Licensed under the Apache License, Version 2.0 (the "License");
**    you may not use this file except in compliance with the License.
**    You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef AT_TOK_H
#define AT_TOK_H 1

int at_tok_start(char **p_cur);
int at_tok_nextint(char **p_cur, int *p_out);
int at_tok_nexthexint(char **p_cur, int *p_out);

int at_tok_nextbool(char **p_cur, char *p_out);
int at_tok_nextstr(char **p_cur, char **out);

int at_tok_hasmore(char **p_cur);
int at_tok_count(const char *in_line);
int at_tok_scanf(const char *line, const char *fmt, ...);

#endif /*AT_TOK_H */

