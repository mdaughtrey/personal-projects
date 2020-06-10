DROP TABLE station;
DROP TABLE wxhistory;


CREATE TABLE station (
        state CHARACTER(2),
        longname TEXT,
        icao CHARACTER(4),
        iata CHARACTER(3),
        synop NUMERIC,
        latitude NUMERIC,
        longitude NUMERIC,
        elevation NUMERIC,
        metar CHARACTER(1),
        nexrad CHARACTER(1),
        airmet CHARACTER(1),
        artcc CHARACTER(1),
        taf CHARACTER(1),
        upperair CHARACTER(1),
        rawinsonde CHARACTER(1),
        windprofiler CHARACTER(1),
        asos CHARACTER(1),
        awos CHARACTER(1),
        meso CHARACTER(1),
        human CHARACTER(1),
        augmented CHARACTER(1),
        office_wfo CHARACTER(1),
        office_rfc CHARACTER(1),
        office_ncep CHARACTER(1),
        country CHARACTER(2)
);

CREATE TABLE wxhistory (
    icao CHARACTER(4),
    observation_time TIMESTAMP WITH TIME ZONE,
    temp_c NUMERIC,
    dewpoint_c NUMERIC,
    wind_dir_degrees NUMERIC,
    wind_speed_kt NUMERIC,
    wind_speed_gust NUMERIC,
    visibility_statute_mi NUMERIC,
    sea_level_pressure_mb NUMERIC,
    quality_control_flags TEXT,
    wx_string TEXT,
    sky_cover TEXT,
    cloud_base_ft_agl NUMERIC,
    flight_category TEXT,
    three_hr_pressure_tendency_mb NUMERIC,
    maxt_c NUMERIC,
    mint_c NUMERIC,
    maxt24hr_c NUMERIC,
    mint24hr_c NUMERIC,
    precip_in NUMERIC,
    pcp3hr_in NUMERIC,
    pcp6hr_in NUMERIC,
    pcp24hr_in NUMERIC,
    snow_in NUMERIC,
    vert_vis_fr NUMERIC
    );

ALTER TABLE wxhistory ADD PRIMARY KEY (icao, observation_time);
ALTER TABLE station ADD PRIMARY KEY (longname,icao,iata,synop);

