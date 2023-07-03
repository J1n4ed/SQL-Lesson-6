// module file with dbase control class

module;

// Core includes
#include <iostream>
#include <memory>

// WT Lib
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>

export module dbase;

namespace jinx
{
// ============================================================================================================================

	// fw delc

	class publisher;
	class book;
	class stock;
	class shop;
	class sale;

	export class DBASE final // --------------------------------------------------------------------- DBASE
	{
	public:
		// PUBLIC AREA

		// public methods

		void find_shops(std::string);

		// overloads

		// constructors

		DBASE() = delete;
		
		DBASE(std::string);

		// destructor

	protected:
		// PROTECTED AREA

	private:
		// PRIVATE AREA

		// vars

		std::string _connectionString;
		Wt::Dbo::Session _session;		

		// methods

		void fill_publishers();
		void fill_books();
		void fill_shops();
		void fill_stocks();
		void fill_sales();		

	}; // !DBASE ------------------------------------------------------------------------------ !DBASE

	//  Î‡ÒÒ˚ Ú‡·ÎËˆ

	/*
	- id (automatic wt)
	- name
	*/
	class publisher final
	{
	public:
		Wt::Dbo::collection < Wt::Dbo::ptr<book> > id;
		std::string name;

		template<class Action>
		void persist(Action& a)
		{
			Wt::Dbo::hasMany(a, id, Wt::Dbo::ManyToOne, "publisher");
			Wt::Dbo::field(a, name, "name");
		}
	}; // !publisher ++++++++++++

	/*
	- id (automatic wt)
	- title
	- id_publisher related publisher(id)
	*/
	class book final
	{
	public:
		Wt::Dbo::collection < Wt::Dbo::ptr<stock> > id;
		std::string title;
		Wt::Dbo::ptr<publisher> publisher;

		template<class Action>
		void persist(Action& a)
		{			
			Wt::Dbo::hasMany(a, id, Wt::Dbo::ManyToMany, "id_book");
			Wt::Dbo::field(a, title, "title");
			Wt::Dbo::belongsTo(a, publisher, "publisher");
		}
	}; // !book +++++++++++++++++

	/*
	- id (automatic wt)
	- id_book related book(id)
	- id_shop related shop(id)
	- count
	*/
	class stock final
	{
	public:
		
		Wt::Dbo::collection< Wt::Dbo::ptr<sale> > id;
		Wt::Dbo::collection< Wt::Dbo::ptr<book> > id_book;
		Wt::Dbo::ptr<shop> shop;
		int count;

		template<class Action>
		void persist(Action& a)
		{
			Wt::Dbo::hasMany(a, id, Wt::Dbo::ManyToOne, "stock");
			Wt::Dbo::hasMany(a, id_book, Wt::Dbo::ManyToMany, "id_book");			
			Wt::Dbo::belongsTo(a, shop, "shop");
			Wt::Dbo::field(a, count, "count");
		}
	}; // !stock ++++++++++++++++

	/*
	- id (automatic wt)
	- name
	*/
	class shop final
	{
	public:
		// Wt::Dbo::ptr < stock > id;
		Wt::Dbo::collection< Wt::Dbo::ptr< stock> > id;
		std::string name;

		template<class Action>
		void persist(Action& a)
		{
			Wt::Dbo::hasMany(a, id, Wt::Dbo::ManyToOne, "shop");
			Wt::Dbo::field(a, name, "name");
		}
	}; // !shop +++++++++++++++++

	/*
	- id (automatic wt)
	- price
	- date_sale
	- id_stock related stock(id)
	- count
	*/
	class sale final
	{
	public:

		int price;
		std::string date_sale;
		Wt::Dbo::ptr<stock> stock;
		int count;

		template<class Action>
		void persist(Action& a)
		{
			Wt::Dbo::field(a, price, "price");
			Wt::Dbo::field(a, date_sale, "date_sale");
			Wt::Dbo::belongsTo(a, stock, "stock");
			Wt::Dbo::field(a, count, "count");
		}
	}; // !sale +++++++++++++++++

	// -------------------- METHODS ------------------------

	/*
	Constructor for DBASE class
	*/
	DBASE::DBASE(std::string connectionString) : _connectionString(connectionString)
	{
		std::cout << "\n> JOB: Creating dbase object...";
		
		auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(_connectionString);		

		try
		{
			_session.setConnection(std::move(postgres));	
		}
		catch (Wt::Dbo::Exception & ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		// MAP BOOK
		try
		{
			_session.mapClass<book>("book");
		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		// MAP PUBLISHER
		try
		{
			_session.mapClass<publisher>("publisher");
		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		// MAP STOCK
		try
		{
			_session.mapClass<stock>("stock");
		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		// MAP SHOP
		try
		{
			_session.mapClass<shop>("shop");
		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		// MAP SALE
		try
		{
			_session.mapClass<sale>("sale");
		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		// MAKE TRANSACTION
		Wt::Dbo::Transaction transaction(_session);

		try
		{
			std::cout << "\n> JOB: Creating tables...";

			// make tables
			_session.createTables();		

			fill_publishers();
			fill_books();
			fill_shops();
			fill_stocks();
			fill_sales();

			std::cout << "\n> Creating tables complete!";

		}
		catch (const Wt::Dbo::Exception& e)
		{
			std::cout << std::endl << "> DBO: Exception while creating tables!\nOutput: " << e.what() << std::endl;
		}	

		transaction.commit();		

	} // !DBASE

	void DBASE::fill_publishers()
	{		
		std::cout << "\n> JOB: Filling publishers...";		

		Wt::Dbo::Transaction transaction(_session);
		
		// publisher	

		std::unique_ptr<publisher> pub_gator(new publisher);
		pub_gator->name = "Alligator Pubs";
		Wt::Dbo::ptr<publisher> pub_1 = _session.add(std::move(pub_gator));

		std::unique_ptr<publisher> pub_rex(new publisher);
		pub_rex->name = "REX Pages";
		Wt::Dbo::ptr<publisher> pub_2 = _session.add(std::move(pub_rex));

		std::unique_ptr<publisher> pub_wolf(new publisher);
		pub_wolf->name = "Wolf Industries";
		Wt::Dbo::ptr<publisher> pub_3 = _session.add(std::move(pub_wolf));
		
		transaction.commit();

		std::cout << "\n> COMPLETE!";
	}

	void DBASE::fill_books()
	{
		// DEBUG ------------
		std::cout << "\n> JOB: Filling books...";
		// !DEBUG -----------
		try
		{

		Wt::Dbo::Transaction transaction(_session);

		// references to publishers
		
		Wt::Dbo::ptr<publisher> pub_1 = _session.find<publisher>().where("name = ?").bind("Alligator Pubs");
		Wt::Dbo::ptr<publisher> pub_2 = _session.find<publisher>().where("name = ?").bind("REX Pages");
		Wt::Dbo::ptr<publisher> pub_3 = _session.find<publisher>().where("name = ?").bind("Wolf Industries");

		// books

		std::unique_ptr<book> book_harry_1(new book);
		book_harry_1->title = "Harry Spotter and Chamber of Gains";
		Wt::Dbo::ptr<book> book_1 = _session.add(std::move(book_harry_1));		
		pub_1.modify()->id.insert(book_1);

		std::unique_ptr<book> book_harry_2(new book);
		book_harry_2->title = "Harry Spotter and Goblet of Whey";
		Wt::Dbo::ptr<book> book_2 = _session.add(std::move(book_harry_2));
		pub_1.modify()->id.insert(book_2);

		std::unique_ptr<book> book_harry_3(new book);
		book_harry_3->title = "Harry Spotter and Gifts of Gym";
		Wt::Dbo::ptr<book> book_3 = _session.add(std::move(book_harry_3));		
		pub_1.modify()->id.insert(book_3);

		//--		

		std::unique_ptr<book> book_herald_1(new book);
		book_herald_1->title = "Geralt of Ryazan, Book 1";
		Wt::Dbo::ptr<book> book_4 = _session.add(std::move(book_herald_1));	
		pub_2.modify()->id.insert(book_4);

		std::unique_ptr<book> book_herald_2(new book);
		book_herald_2->title = "Geralt of Ryazan, Book 2";
		Wt::Dbo::ptr<book> book_5 = _session.add(std::move(book_herald_2));	
		pub_2.modify()->id.insert(book_5);

		std::unique_ptr<book> book_herald_3(new book);
		book_herald_3->title = "Geralt of Ryazan, Book 3";
		Wt::Dbo::ptr<book> book_6 = _session.add(std::move(book_herald_3));	
		pub_2.modify()->id.insert(book_6);

		//--		

		std::unique_ptr<book> book_phyx_1(new book);
		book_phyx_1->title = "Physics for dummies, part 1";
		Wt::Dbo::ptr<book> book_7 = _session.add(std::move(book_phyx_1));	
		pub_3.modify()->id.insert(book_7);

		std::unique_ptr<book> book_phyx_2(new book);
		book_phyx_2->title = "Physics for dummies, part 2";
		Wt::Dbo::ptr<book> book_8 = _session.add(std::move(book_phyx_2));		
		pub_3.modify()->id.insert(book_8);

		std::unique_ptr<book> book_phyx_3(new book);
		book_phyx_3->title = "Physics for dummies, part 3";
		Wt::Dbo::ptr<book> book_9 = _session.add(std::move(book_phyx_3));	
		pub_3.modify()->id.insert(book_9);

		transaction.commit();

		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		std::cout << "\n> COMPLETE!";

	} // !fill_books() -----------------------------------

	void DBASE::fill_shops()
	{
		// DEBUG ------------
		std::cout << "\n> JOB: Filling shops...";
		// !DEBUG -----------

		Wt::Dbo::Transaction transaction(_session);

		// shops	

		std::unique_ptr<shop> my_shop_1(new shop);
		my_shop_1->name = "My Book Store 11";
		Wt::Dbo::ptr<shop> shop_1 = _session.add(std::move(my_shop_1));

		std::unique_ptr<shop> my_shop_2(new shop);
		my_shop_2->name = "Grandbooks";
		Wt::Dbo::ptr<shop> shop_2 = _session.add(std::move(my_shop_2));

		std::unique_ptr<shop> my_shop_3(new shop);
		my_shop_3->name = "Mega Book Mag";
		Wt::Dbo::ptr<shop> shop_3 = _session.add(std::move(my_shop_3));

		transaction.commit();

		std::cout << "\n> COMPLETE!";

	} // !fill_shops()

	void DBASE::fill_stocks()
	{
		// DEBUG ------------
		std::cout << "\n> JOB: Filling stocks...";
		// !DEBUG -----------		

		// references to shops

		// references to publishers

		Wt::Dbo::ptr<shop> shop_1 = _session.find<shop>().where("name = ?").bind("My Book Store 11");
		Wt::Dbo::ptr<shop> shop_2 = _session.find<shop>().where("name = ?").bind("Grandbooks");
		Wt::Dbo::ptr<shop> shop_3 = _session.find<shop>().where("name = ?").bind("Mega Book Mag");

		// references to books

		Wt::Dbo::ptr<book> book_1 = _session.find<book>().where("title = ?").bind("Harry Spotter and Chamber of Gains");
		Wt::Dbo::ptr<book> book_2 = _session.find<book>().where("title = ?").bind("Harry Spotter and Goblet of Whey");
		Wt::Dbo::ptr<book> book_3 = _session.find<book>().where("title = ?").bind("Harry Spotter and Gifts of Gym");
		Wt::Dbo::ptr<book> book_4 = _session.find<book>().where("title = ?").bind("Geralt of Ryazan, Book 1");
		Wt::Dbo::ptr<book> book_5 = _session.find<book>().where("title = ?").bind("Geralt of Ryazan, Book 2");
		Wt::Dbo::ptr<book> book_6 = _session.find<book>().where("title = ?").bind("Geralt of Ryazan, Book 3");
		Wt::Dbo::ptr<book> book_7 = _session.find<book>().where("title = ?").bind("Physics for dummies, part 1");
		Wt::Dbo::ptr<book> book_8 = _session.find<book>().where("title = ?").bind("Physics for dummies, part 2");
		Wt::Dbo::ptr<book> book_9 = _session.find<book>().where("title = ?").bind("Physics for dummies, part 3");

		// stock

		try
		{
			Wt::Dbo::Transaction transaction(_session);			
			
			std::unique_ptr<stock> stock_1(new stock);
			stock_1->count = 100;
			Wt::Dbo::ptr<stock> new_stock_1 = _session.add(std::move(stock_1));
			shop_1.modify()->id.insert(new_stock_1);
			new_stock_1.modify()->id_book.insert(book_1);

			std::unique_ptr<stock> stock_2(new stock);
			stock_2->count = 70;
			Wt::Dbo::ptr<stock> new_stock_2 = _session.add(std::move(stock_2));
			shop_1.modify()->id.insert(new_stock_2);
			new_stock_2.modify()->id_book.insert(book_4);

			std::unique_ptr<stock> stock_3(new stock);
			stock_3->count = 120;
			Wt::Dbo::ptr<stock> new_stock_3 = _session.add(std::move(stock_3));
			shop_1.modify()->id.insert(new_stock_3);
			new_stock_3.modify()->id_book.insert(book_7);

			std::unique_ptr<stock> stock_4(new stock);
			stock_4->count = 35;
			Wt::Dbo::ptr<stock> new_stock_4 = _session.add(std::move(stock_4));
			shop_2.modify()->id.insert(new_stock_4);
			new_stock_4.modify()->id_book.insert(book_2);

			std::unique_ptr<stock> stock_5(new stock);
			stock_5->count = 42;
			Wt::Dbo::ptr<stock> new_stock_5 = _session.add(std::move(stock_5));
			shop_2.modify()->id.insert(new_stock_5);
			new_stock_5.modify()->id_book.insert(book_5);

			std::unique_ptr<stock> stock_6(new stock);
			stock_6->count = 19;
			Wt::Dbo::ptr<stock> new_stock_6 = _session.add(std::move(stock_6));
			shop_2.modify()->id.insert(new_stock_6);
			new_stock_6.modify()->id_book.insert(book_8);

			std::unique_ptr<stock> stock_7(new stock);
			stock_7->count = 56;
			Wt::Dbo::ptr<stock> new_stock_7 = _session.add(std::move(stock_7));
			shop_3.modify()->id.insert(new_stock_7);
			new_stock_7.modify()->id_book.insert(book_3);

			std::unique_ptr<stock> stock_8(new stock);
			stock_8->count = 36;
			Wt::Dbo::ptr<stock> new_stock_8 = _session.add(std::move(stock_8));
			shop_3.modify()->id.insert(new_stock_8);
			new_stock_8.modify()->id_book.insert(book_6);

			std::unique_ptr<stock> stock_9(new stock);
			stock_9->count = 75;
			Wt::Dbo::ptr<stock> new_stock_9 = _session.add(std::move(stock_9));
			shop_3.modify()->id.insert(new_stock_9);
			new_stock_9.modify()->id_book.insert(book_9);

			// extra stock in shop 1

			std::unique_ptr<stock> stock_10(new stock);
			stock_10->count = 81;
			Wt::Dbo::ptr<stock> new_stock_10 = _session.add(std::move(stock_10));
			shop_1.modify()->id.insert(new_stock_10);
			new_stock_10.modify()->id_book.insert(book_2);

			std::unique_ptr<stock> stock_11(new stock);
			stock_11->count = 33;
			Wt::Dbo::ptr<stock> new_stock_11 = _session.add(std::move(stock_11));
			shop_1.modify()->id.insert(new_stock_11);
			new_stock_11.modify()->id_book.insert(book_5);

			// extra stock in shop 2

			std::unique_ptr<stock> stock_12(new stock);
			stock_12->count = 12;
			Wt::Dbo::ptr<stock> new_stock_12 = _session.add(std::move(stock_12));
			shop_2.modify()->id.insert(new_stock_12);
			new_stock_12.modify()->id_book.insert(book_6);

			std::unique_ptr<stock> stock_13(new stock);
			stock_13->count = 61;
			Wt::Dbo::ptr<stock> new_stock_13 = _session.add(std::move(stock_13));
			shop_2.modify()->id.insert(new_stock_13);
			new_stock_13.modify()->id_book.insert(book_3);

			std::unique_ptr<stock> stock_14(new stock);
			stock_14->count = 92;
			Wt::Dbo::ptr<stock> new_stock_14 = _session.add(std::move(stock_14));
			shop_2.modify()->id.insert(new_stock_14);
			new_stock_14.modify()->id_book.insert(book_9);

			// extra stock in shop 3

			std::unique_ptr<stock> stock_15(new stock);
			stock_15->count = 15;
			Wt::Dbo::ptr<stock> new_stock_15 = _session.add(std::move(stock_15));
			shop_3.modify()->id.insert(new_stock_15);
			new_stock_15.modify()->id_book.insert(book_5);

			transaction.commit();
		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}

		std::cout << "\n> COMPLETE!";
		
	} // !fill_stocks()	

	void DBASE::fill_sales()
	{		
		std::cout << "\n> JOB: filling sales...";		

		Wt::Dbo::Transaction transaction(_session);

		// stock references

		Wt::Dbo::ptr<stock> stock_1 = _session.find<stock>().where("id = ?").bind(1);
		Wt::Dbo::ptr<stock> stock_2 = _session.find<stock>().where("id = ?").bind(2);
		Wt::Dbo::ptr<stock> stock_3 = _session.find<stock>().where("id = ?").bind(3);
		Wt::Dbo::ptr<stock> stock_4 = _session.find<stock>().where("id = ?").bind(4);
		Wt::Dbo::ptr<stock> stock_5 = _session.find<stock>().where("id = ?").bind(5);
		Wt::Dbo::ptr<stock> stock_6 = _session.find<stock>().where("id = ?").bind(6);
		Wt::Dbo::ptr<stock> stock_7 = _session.find<stock>().where("id = ?").bind(7);
		Wt::Dbo::ptr<stock> stock_8 = _session.find<stock>().where("id = ?").bind(8);
		Wt::Dbo::ptr<stock> stock_9 = _session.find<stock>().where("id = ?").bind(9);
		Wt::Dbo::ptr<stock> stock_10 = _session.find<stock>().where("id = ?").bind(10);
		Wt::Dbo::ptr<stock> stock_11 = _session.find<stock>().where("id = ?").bind(11);
		Wt::Dbo::ptr<stock> stock_12 = _session.find<stock>().where("id = ?").bind(12);
		Wt::Dbo::ptr<stock> stock_13 = _session.find<stock>().where("id = ?").bind(13);
		Wt::Dbo::ptr<stock> stock_14 = _session.find<stock>().where("id = ?").bind(14);
		Wt::Dbo::ptr<stock> stock_15 = _session.find<stock>().where("id = ?").bind(15);

		try
		{
			Wt::Dbo::Transaction transaction(_session);

			// sales (30 entrees)		

			std::unique_ptr<sale> sale_1(new sale);
			sale_1->count = 1;
			sale_1->date_sale = "15/06/2023";
			sale_1->price = 300;
			Wt::Dbo::ptr<sale> new_sale_1 = _session.add(std::move(sale_1));
			stock_1.modify()->id.insert(new_sale_1);

			std::unique_ptr<sale> sale_2(new sale);
			sale_2->count = 2;
			sale_2->date_sale = "15/06/2023";
			sale_2->price = 800;
			Wt::Dbo::ptr<sale> new_sale_2 = _session.add(std::move(sale_2));
			stock_2.modify()->id.insert(new_sale_2);

			std::unique_ptr<sale> sale_3(new sale);
			sale_3->count = 3;
			sale_3->date_sale = "15/06/2023";
			sale_3->price = 1300;
			Wt::Dbo::ptr<sale> new_sale_3 = _session.add(std::move(sale_3));
			stock_3.modify()->id.insert(new_sale_3);

			std::unique_ptr<sale> sale_4(new sale);
			sale_4->count = 1;
			sale_4->date_sale = "16/06/2023";
			sale_4->price = 350;
			Wt::Dbo::ptr<sale> new_sale_4 = _session.add(std::move(sale_4));
			stock_4.modify()->id.insert(new_sale_4);

			std::unique_ptr<sale> sale_5(new sale);
			sale_5->count = 2;
			sale_5->date_sale = "16/06/2023";
			sale_5->price = 500;
			Wt::Dbo::ptr<sale> new_sale_5 = _session.add(std::move(sale_5));
			stock_5.modify()->id.insert(new_sale_5);

			std::unique_ptr<sale> sale_6(new sale);
			sale_6->count = 3;
			sale_6->date_sale = "17/06/2023";
			sale_6->price = 2300;
			Wt::Dbo::ptr<sale> new_sale_6 = _session.add(std::move(sale_6));
			stock_6.modify()->id.insert(new_sale_6);

			std::unique_ptr<sale> sale_7(new sale);
			sale_7->count = 1;
			sale_7->date_sale = "17/06/2023";
			sale_7->price = 200;
			Wt::Dbo::ptr<sale> new_sale_7 = _session.add(std::move(sale_7));
			stock_7.modify()->id.insert(new_sale_7);

			std::unique_ptr<sale> sale_8(new sale);
			sale_8->count = 2;
			sale_8->date_sale = "17/06/2023";
			sale_8->price = 650;
			Wt::Dbo::ptr<sale> new_sale_8 = _session.add(std::move(sale_8));
			stock_8.modify()->id.insert(new_sale_8);

			std::unique_ptr<sale> sale_9(new sale);
			sale_9->count = 3;
			sale_9->date_sale = "17/06/2023";
			sale_9->price = 900;
			Wt::Dbo::ptr<sale> new_sale_9 = _session.add(std::move(sale_9));
			stock_9.modify()->id.insert(new_sale_9);

			std::unique_ptr<sale> sale_10(new sale);
			sale_10->count = 1;
			sale_10->date_sale = "18/06/2023";
			sale_10->price = 350;
			Wt::Dbo::ptr<sale> new_sale_10 = _session.add(std::move(sale_10));
			stock_10.modify()->id.insert(new_sale_10);

			std::unique_ptr<sale> sale_11(new sale);
			sale_11->count = 2;
			sale_11->date_sale = "18/06/2023";
			sale_11->price = 750;
			Wt::Dbo::ptr<sale> new_sale_11 = _session.add(std::move(sale_11));
			stock_11.modify()->id.insert(new_sale_11);

			std::unique_ptr<sale> sale_12(new sale);
			sale_12->count = 3;
			sale_12->date_sale = "19/06/2023";
			sale_12->price = 1250;
			Wt::Dbo::ptr<sale> new_sale_12 = _session.add(std::move(sale_12));
			stock_12.modify()->id.insert(new_sale_12);

			std::unique_ptr<sale> sale_13(new sale);
			sale_13->count = 1;
			sale_13->date_sale = "20/06/2023";
			sale_13->price = 150;
			Wt::Dbo::ptr<sale> new_sale_13 = _session.add(std::move(sale_13));
			stock_13.modify()->id.insert(new_sale_13);

			std::unique_ptr<sale> sale_14(new sale);
			sale_14->count = 2;
			sale_14->date_sale = "21/06/2023";
			sale_14->price = 500;
			Wt::Dbo::ptr<sale> new_sale_14 = _session.add(std::move(sale_14));
			stock_14.modify()->id.insert(new_sale_14);

			std::unique_ptr<sale> sale_15(new sale);
			sale_15->count = 3;
			sale_15->date_sale = "21/06/2023";
			sale_15->price = 2500;
			Wt::Dbo::ptr<sale> new_sale_15 = _session.add(std::move(sale_15));
			stock_15.modify()->id.insert(new_sale_15);

			std::unique_ptr<sale> sale_16(new sale);
			sale_16->count = 1;
			sale_16->date_sale = "23/06/2023";
			sale_16->price = 800;
			Wt::Dbo::ptr<sale> new_sale_16 = _session.add(std::move(sale_16));
			stock_15.modify()->id.insert(new_sale_16);

			std::unique_ptr<sale> sale_17(new sale);
			sale_17->count = 2;
			sale_17->date_sale = "23/06/2023";
			sale_17->price = 900;
			Wt::Dbo::ptr<sale> new_sale_17 = _session.add(std::move(sale_17));
			stock_14.modify()->id.insert(new_sale_17);

			std::unique_ptr<sale> sale_18(new sale);
			sale_18->count = 3;
			sale_18->date_sale = "25/06/2023";
			sale_18->price = 900;
			Wt::Dbo::ptr<sale> new_sale_18 = _session.add(std::move(sale_18));
			stock_13.modify()->id.insert(new_sale_18);

			std::unique_ptr<sale> sale_19(new sale);
			sale_19->count = 1;
			sale_19->date_sale = "25/06/2023";
			sale_19->price = 500;
			Wt::Dbo::ptr<sale> new_sale_19 = _session.add(std::move(sale_19));
			stock_12.modify()->id.insert(new_sale_19);

			std::unique_ptr<sale> sale_20(new sale);
			sale_20->count = 2;
			sale_20->date_sale = "25/06/2023";
			sale_20->price = 1000;
			Wt::Dbo::ptr<sale> new_sale_20 = _session.add(std::move(sale_20));
			stock_11.modify()->id.insert(new_sale_20);

			std::unique_ptr<sale> sale_21(new sale);
			sale_21->count = 3;
			sale_21->date_sale = "30/06/2023";
			sale_21->price = 1500;
			Wt::Dbo::ptr<sale> new_sale_21 = _session.add(std::move(sale_21));
			stock_10.modify()->id.insert(new_sale_21);

			std::unique_ptr<sale> sale_22(new sale);
			sale_22->count = 1;
			sale_22->date_sale = "30/06/2023";
			sale_22->price = 400;
			Wt::Dbo::ptr<sale> new_sale_22 = _session.add(std::move(sale_22));
			stock_9.modify()->id.insert(new_sale_22);

			std::unique_ptr<sale> sale_23(new sale);
			sale_23->count = 2;
			sale_23->date_sale = "01/07/2023";
			sale_23->price = 800;
			Wt::Dbo::ptr<sale> new_sale_23 = _session.add(std::move(sale_23));
			stock_8.modify()->id.insert(new_sale_23);

			std::unique_ptr<sale> sale_24(new sale);
			sale_24->count = 3;
			sale_24->date_sale = "01/07/2023";
			sale_24->price = 1200;
			Wt::Dbo::ptr<sale> new_sale_24 = _session.add(std::move(sale_24));
			stock_7.modify()->id.insert(new_sale_24);

			std::unique_ptr<sale> sale_25(new sale);
			sale_25->count = 1;
			sale_25->date_sale = "02/07/2023";
			sale_25->price = 150;
			Wt::Dbo::ptr<sale> new_sale_25 = _session.add(std::move(sale_25));
			stock_6.modify()->id.insert(new_sale_25);

			std::unique_ptr<sale> sale_26(new sale);
			sale_26->count = 2;
			sale_26->date_sale = "03/07/2023";
			sale_26->price = 300;
			Wt::Dbo::ptr<sale> new_sale_26 = _session.add(std::move(sale_26));
			stock_5.modify()->id.insert(new_sale_26);

			std::unique_ptr<sale> sale_27(new sale);
			sale_27->count = 3;
			sale_27->date_sale = "03/07/2023";
			sale_27->price = 450;
			Wt::Dbo::ptr<sale> new_sale_27 = _session.add(std::move(sale_27));
			stock_4.modify()->id.insert(new_sale_27);

			std::unique_ptr<sale> sale_28(new sale);
			sale_28->count = 1;
			sale_28->date_sale = "05/07/2023";
			sale_28->price = 200;
			Wt::Dbo::ptr<sale> new_sale_28 = _session.add(std::move(sale_28));
			stock_3.modify()->id.insert(new_sale_28);

			std::unique_ptr<sale> sale_29(new sale);
			sale_29->count = 2;
			sale_29->date_sale = "06/07/2023";
			sale_29->price = 400;
			Wt::Dbo::ptr<sale> new_sale_29 = _session.add(std::move(sale_29));
			stock_2.modify()->id.insert(new_sale_29);

			std::unique_ptr<sale> sale_30(new sale);
			sale_30->count = 3;
			sale_30->date_sale = "06/07/2023";
			sale_30->price = 600;
			Wt::Dbo::ptr<sale> new_sale_30 = _session.add(std::move(sale_30));
			stock_1.modify()->id.insert(new_sale_30);

			transaction.commit();
		}
		catch (Wt::Dbo::Exception& ex)
		{
			std::cout << "\n> DBO: " << ex.what();
		}
		
		std::cout << "\n> COMPLETE!";		

	} // !fill_sales()

	// ====================== «¿ƒ¿Õ»≈ 2 =======================

	void DBASE::find_shops(std::string pubname)
	{
		Wt::Dbo::Transaction transaction(_session);

		Wt::Dbo::collection < Wt::Dbo::ptr<publisher> > pubs = _session.find<publisher>().where("name = ?").bind(pubname);

		std::cout << "\n------------ Looking for publishers and stores ----------------\n";
		std::cout << "\nPublisher: " << pubname;
		std::cout << "\nNumber of entrees: " << pubs.size() << '\n';

		for (const auto& elem : pubs)
		{			
			std::cout << "\n> Found books and shops:\n";
			
			for (const auto& elem2 : elem->id)
			{
				std::cout << "\n- " << elem2->title;

				// Wt::Dbo::collection < Wt::Dbo::ptr < stock > > stocks = _session.find<stock>().where("stock_id = ?").bind(elem2->id);
				std::cout << "\n> Shops: \n";

				for (const auto& elem3 : elem2->id)
				{			

					Wt::Dbo::collection < Wt::Dbo::ptr <shop> > shops = _session.find<shop>().where("id = ?").bind(elem3->shop);

					for (const auto& elem4 : shops)
					{
						std::cout << "- " << elem4->name;
					}

					std::cout << std::endl;
				}
				
				std::cout << std::endl;
			}

			std::cout << std::endl;
		}
		std::cout << std::endl;

	} // !find_shops(std::string pubname)

// ============================================================================================================================
} // !JINX