// dump1090, a Mode S messages decoder for RTLSDR devices.
//
// Copyright (C) 2012 by Salvatore Sanfilippo <antirez@gmail.com>
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//  *  Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//
//  *  Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  mysql.c
//
//
//  Created by Dean Pemberton on 25/09/13.
//
//

#include "dump1090.h"
//
// ============================= Utility functions ==========================
//


void populateairframeMySQL(struct aircraft *a) {
    MYSQL *conn;
    conn = mysql_init(NULL);
    mysql_real_connect(conn, "127.0.0.1", "pi", "raspberry", "dump1090", 0, NULL, 0);
    
    
    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }
    
    
    snprintf(p, 999, "SELECT regn,type from airframe WHERE icao = '%02X'",
             a->addr);
    
    mysql_query(conn, p);
    
    MYSQL_RES *result = mysql_store_result(conn);
    
    if (result == NULL)
    {
        finish_with_error(conn);
    }
    int num_fields = mysql_num_fields(result);
    
    if ( !num_fields) {
         strncpy(a->regn, "UNKNOWN", sizeof("UNKNOWN"));
        strncpy(a->type, "UNKNOWN", sizeof("UNKNOWN"));
    } else {
        MYSQL_ROW row;
        strncpy(a->regn, row[0], sizeof(row[0]));
        strncpy(a->type, row[1], sizeof(row[0]));
    }
    mysql_free_result(result);
    mysql_close(conn);
    
}


/* Write aircraft data to a MySQL Database */
void modesFeedMySQL(struct modesMessage *mm, struct aircraft *a) {
    
    MYSQL *conn;
    conn = mysql_init(NULL);
    mysql_real_connect(conn, "127.0.0.1", "pi", "raspberry", "dump1090", 0, NULL, 0);
    
    char msgFlights[1000], *p = msgFlights;
    
    /* we flill a live 'flights' table - update old data */
    /* DF 0 (Short Air to Air, ACAS has: altitude, icao) */
    if (mm->msgtype == 0) {
        snprintf(p, 999, "INSERT INTO flights (icao, alt, df, msgs) VALUES ('%02X', '%d', '%d', '%ld') "
                 "ON DUPLICATE KEY UPDATE "
                 "icao=VALUES(icao), alt=VALUES(alt), df=VALUES(df), msgs=VALUES(msgs)",
                 mm->addr, mm->altitude, mm->msgtype, a->messages);
        if (mysql_query(conn, p)) {
            printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
            exit(1);
        }
    }
    /* DF 4/20 (Surveillance (roll call) Altitude has: altitude, icao, flight status, DR, UM) */
    /* TODO flight status, DR, UM */
    if (mm->msgtype == 4 || mm->msgtype == 20){
        snprintf(p, 999, "INSERT INTO flights (icao, alt, df, msgs) VALUES ('%02X', '%d', '%d', '%ld') "
                 "ON DUPLICATE KEY UPDATE icao=VALUES(icao), alt=VALUES(alt), df=VALUES(df), msgs=VALUES(msgs)",
                 mm->addr, mm->altitude, mm->msgtype, a->messages);
        if (mysql_query(conn, p)) {
            printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
            exit(1);
        }
    }
    /* DF 5/21 (Surveillance (roll call) IDENT Reply, has: alt, icao, flight status, DR, UM, squawk) */
    if (mm->msgtype == 5 || mm->msgtype == 21) {
        snprintf(p, 999, "INSERT INTO flights (icao, alt, squawk, df, msgs) VALUES ('%02X', '%d', '%d', '%d', '%ld') "
                 "ON DUPLICATE KEY UPDATE icao=VALUES(icao), alt=VALUES(alt), squawk=VALUES(squawk), df=VALUES(df), "
                 "msgs=VALUES(msgs)",
                 mm->addr, mm->altitude, mm->modeA, mm->msgtype, a->messages);
        if (mysql_query(conn, p)) {
            printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
            exit(1);
        }
    }
    /* DF 11 */
    if (mm->msgtype == 11) {
        snprintf(p, 999, "INSERT INTO flights (icao, df, msgs) VALUES ('%02X', '%d', '%ld') "
                 "ON DUPLICATE KEY UPDATE icao=VALUES(icao), df=VALUES(df), msgs=VALUES(msgs)",
                 mm->addr, mm->msgtype, a->messages);
        if (mysql_query(conn, p)) {
            printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
            exit(1);
        }
        //mysql_close(conn);
    }
    /* DF17 *with or without position data */
    if (mm->msgtype == 17) {
        snprintf(p, 999, "INSERT INTO flights (df, flight, airline, icao, alt, vr, lat, lon, speed, heading, msgs) "
                 "VALUES ('%d', '%s', '%3s', '%02X', '%d', '%d', '%1.5f', '%1.5f', '%d', '%d', '%ld') "
                 "ON DUPLICATE KEY UPDATE "
                 "df=VALUES(df), flight=VALUES(flight), airline=VALUES(airline), icao=VALUES(icao), alt=VALUES(alt), vr=VALUES(vr), "
                 "lat=VALUES(lat), lon=VALUES(lon), speed=VALUES(speed), heading=VALUES(heading), msgs=VALUES(msgs)",
                 mm->msgtype, a->flight, a->flight, mm->addr, mm->altitude, mm->vert_rate, a->lat, a->lon,
                 a->speed, a->track, a->messages);
        if (mysql_query(conn, p)) {
            printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
            exit(1);
        }
    }
    /* update 'tracks' table if we have position data (df 17 extended squitter with position) */
    if (mm->msgtype == 17 && mm->metype >= 9 && mm->metype <= 18) {
        if (a->lat != 0 && a->lon != 0) {
            snprintf(p, 999, "INSERT INTO tracks (icao, alt, lat , lon) VALUES ('%02X','%d','%1.5f','%1.5f')",
                     mm->addr, mm->altitude, a->lat, a->lon);
            if (mysql_query(conn, p)) {
                printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
                exit(1);
            }
        }
    }
    mysql_close(conn);
}

