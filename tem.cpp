// To input data into database
void input_db_data(){
	string inp_query = "";
	MySql db;
	connect_with_db(db);
	for(int i=1;i<=10;i++){
		inp_query = "";
		inp_query.append("insert into ue_autn values(");
		inp_query.append(to_char_array(i+100));
		inp_query.append(", ");
		inp_query.append(to_char_array(i+25));
		inp_query.append(")");
		g_query = inp_query.c_str();
		db.perform_query(g_query);
	}
	g_query = "select autn, rand from ue_autn";
	db.perform_query(g_query);
	db.fetch_result();
}

